/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>


namespace Xmaxplatform {
namespace Chain {
	enum account_type : uint8_t
	{
		acc_personal = 0,
		acc_system = 1,
		acc_contract = 2,
		acc_erc20 = 10,
		acc_erc721 = 11,
	};

}
}
FC_REFLECT_ENUM(Xmaxplatform::Chain::account_type, (acc_personal)(acc_contract)(acc_erc20)(acc_erc721))