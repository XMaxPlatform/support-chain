/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <cash/cash_types.hpp>
#include <vector>

namespace Xmaxplatform {
namespace Chain {

	struct cash_input {
		cash_input() = default;
		cash_input(const uint32_t& num, const uint16_t& slot)
			: num(num), slot(slot) {}

		uint32_t	num = 0;
		uint16_t	slot = 0;
	};

	struct cash_output {
		cash_output() = default;
		cash_output(const uint64_t& amount, const address& to)
			: amount(amount), to(to) {}

		uint64_t	amount = 0;
		address		to;
	};

	struct cash_detail 
	{
		std::vector<cash_input> inputs;
		std::vector<cash_output> outputs;

		cash_signature sig;
	};

}
}