/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <fc/time.hpp>
#include <basechain.hpp>
namespace Xmaxplatform {
namespace Chain {

	class chain_xmax;
	class signed_transaction;

	using Basechain::database;

	class transaction_context_xmax
	{
	public:
		transaction_context_xmax(chain_xmax& _chain, const signed_transaction& _trx, fc::time_point _start = fc::time_point::now());


	public:
		chain_xmax&						chain;
		const signed_transaction&		trx;

		database::session			dbsession;
		fc::time_point	start_time;
	};
}
}
