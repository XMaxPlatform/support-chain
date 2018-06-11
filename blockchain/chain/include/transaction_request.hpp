/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <transaction.hpp>

namespace Xmaxplatform { namespace Chain {

	class transaction_request
	{
	public:
		signed_transaction signed_trx;


		transaction_request(const signed_transaction& trx) 
		: signed_trx(trx) {
				
		}
	};

	typedef std::shared_ptr<transaction_request> transaction_request_ptr;
}
}