/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <chain_xmax.hpp>
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
}
}