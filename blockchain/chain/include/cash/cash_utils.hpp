/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <blockchain_config.hpp>
#include <cash/cash_mint.hpp>

namespace Xmaxplatform {
	namespace Basetypes
	{
		struct cash_detail;
	}
	namespace Chain {

		namespace utils
		{
			void check_cash_from_mint(const cash_mint& mint, const Basetypes::cash_detail& cash);
		}
	}
}