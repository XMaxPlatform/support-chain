/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <cash/cash_types.hpp>
#include <vector>

namespace Xmaxplatform {
namespace Chain {

	enum paytype : uint16_t
	{
		pay_to_addr = 0,
		mint_to_addr = 1,
	};


	struct cash_input {
		cash_input() = default;
		cash_input(const cash_digest& h, const uint8_t& slot)
			: prevout(h), slot(slot) {}

		cash_digest	prevout;
		uint8_t		slot = 0;
	};

	using cash_inputs = std::vector<cash_input>;

	struct cash_output {
		cash_output()
			: to(address::addr_zero)
		{

		}
		cash_output(const uint64_t& amount, const address& to)
			: amount(amount), to(to) {}

		cash_token	amount = 0;
		address		to;
	};
	using cash_outputs = std::vector<cash_output>;


	struct cash_attachment {
		cash_attachment() = default;
		cash_attachment(const chain_timestamp& locktime)
			: locktime(locktime) {}

		chain_timestamp locktime;
	};

	struct cash_detail 
	{
		paytype pay;
		cash_inputs inputs;
		cash_outputs outputs;
		cash_attachment attachment;
		cash_signature sig;

		cash_detail(paytype pt, const  Basetypes::pay_cash&  cash);
	};

	//template<typename Stream>
	//inline void pack_cash(stream& s, const cash_detail& cash)
	//{
	//	switch (cash.pay)
	//	{
	//	case paytype::mint_to_addr:
	//	{

	//	}
	//	default:
	//		break;
	//	}
	//}

}
}