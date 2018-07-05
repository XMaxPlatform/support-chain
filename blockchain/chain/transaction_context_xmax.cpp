/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <chain_xmax.hpp>
#include <vm_xmax.hpp>
#include <transaction_context_xmax.hpp>

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
			exec_message(msg);
		}
	}

	void transaction_context_xmax::squash()
	{
		dbsession.squash();
	}

	void transaction_context_xmax::exec_message(const Chain::message_xmax& msg)
	{
		message_context_xmax xmax_ctx(chain, chain.get_mutable_database(), trx, msg, msg.code);

		try {
			
			auto handler = chain.find_message_handler(xmax_ctx.code, xmax_ctx.msg.type);
			if (handler)
			{
				handler(xmax_ctx);
			}
			else
			{
				const auto& recipient = xmax_ctx.db.get<account_object, by_name>(xmax_ctx.code);
				if (recipient.code.size()) {
					idump((xmax_ctx.code)(xmax_ctx.msg.type));
					const uint32_t execution_time = 10000;//TODO
					try {
						vm_xmax::get().apply(xmax_ctx, execution_time, true);
					}
					catch (const fc::exception &ex) {

					}
				}

			}
		} FC_CAPTURE_AND_RETHROW((xmax_ctx.msg))
	}
}
}