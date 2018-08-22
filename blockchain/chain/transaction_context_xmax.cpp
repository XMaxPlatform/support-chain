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

		message_context_xmax xmax_ctx(chain, chain.get_mutable_database(), trx, msg, apply_depth);

		xmax_ctx.notified.push_back(msg.code);

		message_response res = exec_one_message(xmax_ctx);

		XMAX_ASSERT(xmax_ctx.notified.size() < 1000,
			transaction_exception, "to many recipient.");

		response->message_responses.emplace_back(std::move(res));

		for ( int i = 1; i < xmax_ctx.notified.size(); ++i)
		{
			account_name notify_code = xmax_ctx.notified[i];
			xmax_ctx.change_code(notify_code);
			message_response res2 = exec_one_message(xmax_ctx);

			response->message_responses.emplace_back(std::move(res2));
		}

		for (const auto& it : xmax_ctx.inline_messages)
		{
			exec_message(it, apply_depth + 1);
		}
	}

	message_response transaction_context_xmax::exec_one_message(message_context_xmax& context)
	{
		try {
			const account_object& acc = context.db.get<account_object, by_name>(context.code);

			if (acc.type != acc_contract)
			{
				native_scope scope = get_native_scope(acc.type);
				XMAX_ASSERT(scope != native_scope::native_invalid, transaction_exception,
					"Unknown native scope type '${type}' of account '${acc_name}'.", ("type", (int)scope)("acc_name", acc.name.to_string()));

				auto handler = chain.find_native_handler(scope, context.msg.type);
				XMAX_ASSERT(handler, transaction_exception, "There is not function '${name}' in account '${acc_name}'.", ("name", context.msg.type)("acc_name", acc.name.to_string()));
				handler(context);
			}
			else
			{				
				XMAX_ASSERT(acc.contract, transaction_exception, "contract of '${name}' is not found.", ("name", context.code.to_string()));
	
				idump((context.code)(context.msg.type));
				const uint32_t execution_time = 10000;//TODO
				try {
					vm_xmax::get().apply(context, execution_time, true);
				}
				FC_CAPTURE_AND_LOG((context.msg))		
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