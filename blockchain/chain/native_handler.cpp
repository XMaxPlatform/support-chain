/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */


#include <native_handler.hpp>

namespace Xmaxplatform { namespace Chain {

	native_scope get_native_scope(account_type acc_type)
	{
		switch (acc_type)
		{
		case account_type::acc_personal:
		{
			return native_scope::native_system;
		}
		case account_type::acc_system:
		{
			return native_scope::native_system;
		}
		case account_type::acc_erc20:
		{
			return native_scope::native_erc20;
		}
		case account_type::acc_erc721:
		{
			return native_scope::native_erc721;
		}
		default:
		{
			return native_scope::native_invalid;
		}
		}
	}

} } // namespace Xmaxplatform::Chain
