/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <xmax_contract.hpp>


namespace Xmaxplatform {
namespace Native_contract {


	void xmax_cash_mint(Chain::message_context_xmax& context)
	{
		Types::mintcash msg = context.msg.as<Types::mintcash>();

		cash_mint mint(msg.mintdetail.sequence, msg.mintdetail.owner, msg.mintdetail.amount);

		cash_digest mintdigest = mint.digest();

		//XMAX_ASSERT(linked != nullptr, message_validate_exception, "Linked permission not found");

	}

}
}