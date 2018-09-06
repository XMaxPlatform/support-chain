/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <xmax_contract.hpp>


namespace Xmaxplatform {
namespace Native_contract {


	void xmax_cash_mint(Chain::message_context_xmax& context)
	{
		Types::mintcash mint = context.msg.as<Types::mintcash>();

		cash_mint cm;
		cm.minter = mint

	}

}
}