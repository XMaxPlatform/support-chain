/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <cash/cash_detail.hpp>
#include <cash/cash_mint.hpp>

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		void check_mint(const cash_mint& mint, const Basetypes::cash_detail&);
	}
}
}