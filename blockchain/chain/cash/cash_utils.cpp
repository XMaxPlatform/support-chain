/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <cash/cash_utils.hpp>
#include <gentypes.hpp>

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		void check_cash_from_mint(const cash_mint& mint, const Basetypes::pay_cash& cash)
		{
			cash_detail detail(paytype::mint_to_addr, cash);
		}

	}
}
}