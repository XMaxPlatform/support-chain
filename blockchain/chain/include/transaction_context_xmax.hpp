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

	using session = Basechain::database::session;

	class transaction_context_xmax
	{
	public:



	public:
		chain_xmax&				chain;
		signed_transaction&		transaction;

		session			db_session;
		fc::time_point	start_time;
	};
}
}
