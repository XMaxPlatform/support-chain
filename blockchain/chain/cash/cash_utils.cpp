/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <cash/cash_utils.hpp>
#include <chain_utils.hpp>
#include <gentypes.hpp>

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		void check_cash_from_mint(const cash_digest& predigest, const address& owner, const Basetypes::pay_cash& cash)
		{
			XMAX_ASSERT(cash.inputs.size() == 1, message_validate_exception, "cash.inputs.size() must be 1.");
			XMAX_ASSERT(cash.outputs.size() == 1, message_validate_exception, "cash.outputs.size() must be 1.");

			cash_digest prevout(cash.inputs[0].prevout);

			XMAX_ASSERT(prevout == predigest, message_validate_exception, "cash.inputs.size() must be 1.");

			cash_detail detail(paytype::mint_to_addr, cash);

			cash_digest digest = detail.digest();

			// check mint sign.

			cash_address addr = utils::to_address(cash.sig, digest);

			XMAX_ASSERT(addr == owner, message_precondition_exception, "bad sig for cash owner.");

		}

	}
}
}