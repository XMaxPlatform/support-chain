/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <cash/cash_mint.hpp>

namespace Xmaxplatform {
namespace Chain {

	cash_digest cash_mint::digest() const
	{
		fc::sha256::encoder e;
		fc::raw::pack(e, sequence);
		fc::raw::pack(e, owner);
		fc::raw::pack(e, token);
		return e.result();
	}

	cash_signature cash_mint::sign(const fc::ecc::private_key& signer) const
	{
		return signer.sign_compact(digest());
	}

}
}