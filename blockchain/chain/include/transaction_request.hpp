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

		static inline bool sort_by_gas(const std::shared_ptr<transaction_request>& ltrx, const std::shared_ptr<transaction_request>& rtrx)
		{
			return ltrx->signed_trx.gas < rtrx->signed_trx.gas;
		}
	};

	template<class trxptr>
	class gas_sort
	{
	public:
		gas_sort() = default;

		constexpr bool operator()(const trxptr& _Left, const trxptr& _Right) const
		{	// apply operator< to operands
			return (_Left->signed_trx.gas < _Right->signed_trx.gas);
		}
	
	};
	typedef std::shared_ptr<transaction_request> transaction_request_ptr;


}
}