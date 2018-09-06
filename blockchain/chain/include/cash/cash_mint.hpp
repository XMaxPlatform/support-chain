/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <cash/cash_types.hpp>
#include <vector>

namespace Xmaxplatform {
namespace Chain {

	struct cash_mint
	{
		uint32_t sequence = 0; // sequence for owner.
		address minter;
		cash_token token;
		cash_digest digest() const;

		cash_signature sign(const fc::ecc::private_key& signer) const;
	};
}
}
