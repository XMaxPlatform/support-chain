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

		transaction_package package;

		transaction_request(const signed_transaction& trx) 
		: package(trx) {
			signed_trx = package.unpack_trx();
		}

		transaction_request(const transaction_package& p)
			: package(p) {
			signed_trx = package.unpack_trx();
		}
	};

	typedef std::shared_ptr<transaction_request> transaction_request_ptr;
}
}