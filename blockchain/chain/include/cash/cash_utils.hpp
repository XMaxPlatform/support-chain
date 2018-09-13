/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <blockchain_config.hpp>
#include <gentypes.hpp>
#include <cash/cash_mint.hpp>
#include <cash/cash_detail.hpp>

namespace Xmaxplatform {

	namespace Chain {

		namespace utils
		{
			void check_cash_from_mint(const cash_digest& predigest, const address& owner, const Basetypes::pay_cash& cash);
		}
	}
}