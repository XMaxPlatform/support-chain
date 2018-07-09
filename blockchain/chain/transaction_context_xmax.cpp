/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <chain_xmax.hpp>
#include <vm_xmax.hpp>
#include <transaction_context_xmax.hpp>
#include <objects/global_status_objects.hpp>

namespace Xmaxplatform {
namespace Chain {

	transaction_context_xmax::transaction_context_xmax(chain_xmax& _chain, const signed_transaction& _trx, fc::time_point _start /* = fc::time_point::now() */)
		: chain(_chain)
		, trx(_trx)
		, start_time(_start)
		, dbsession(_chain.get_mutable_database().start_undo_session(true))
	{

	}

	void transaction_context_xmax::exec()
	{
		response = std::make_shared<transaction_response>();

		for (const auto& msg : trx.messages)
		{
			const Chain::message_xmax xmxmsg(msg);
			exec_message(xmxmsg, 0);
		}
	}

	void transaction_context_xmax::squash()
	{
		dbsession.squash();
	}

	void transaction_context_xmax::exec_message(const Chain::message_xmax& msg, uint32_t apply_depth)
	{

		XMAX_ASSERT(apply_depth < Config::max_message_apply_depth,
			transaction_exception, "inline action recursion depth reached");

		message_context_xmax xmax_ctx(chain, chain.get_mutable_database(), trx, msg, msg.code, apply_depth);

		message_response res = exec_one_message(xmax_ctx);

		response->message_responses.emplace_back(std::move(res));

		for (const auto& it : xmax_ctx.inline_messages)
		{
			exec_message(it, apply_depth + 1);
		}
	}

	message_response transaction_context_xmax::exec_one_message(message_context_xmax& context)
	{
		try {
			
			auto handler = chain.find_message_handler(context.code, context.msg.type);
			if (handler)
			{
				handler(context);
			}
			else
			{
				const auto& recipient = context.db.get<account_object, by_name>(context.code);
				if (recipient.code.size()) {
					idump((context.code)(context.msg.type));
					const uint32_t execution_time = 10000;//TODO
					try {
						vm_xmax::get().apply(context, execution_time, true);
					}
					catch (const fc::exception &ex) {

					}
				}

			}
		} FC_CAPTURE_AND_RETHROW((context.msg))

		uint64_t idx = 0;
		{
			const auto& obj = context.mutable_db.get<global_msg_status_object>();
			context.mutable_db.modify(obj, [&](auto& mrs) {
				++mrs.counter;
			});
			idx = obj.counter;
		}
		message_receipt receipt;
		receipt.to_code = context.code;
		receipt.message_idx = idx;
		receipt.message_id = xmax_type_message_id::hash(context.msg);

		msg_receipts.push_back(receipt);

		message_response msg;
		msg.msg_receipt = receipt;
		msg.msg_body = context.msg;
		msg.owner_id = trx.id();

		return msg;
	}
}
}