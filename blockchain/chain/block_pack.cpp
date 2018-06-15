/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <blockchain_exceptions.hpp>
#include <block_pack.hpp>

namespace Xmaxplatform {
namespace Chain {

	void block_raw::add_confirmation(const block_confirmation& conf, uint32_t skip)
	{
		if (NO_BIT_FLAG(skip, Config::skip_confirmation))
		{
			for (const auto& item : confirmations)
			{
				FC_ASSERT(item.verifier != conf.verifier, "confirmation had exist.");
			}

			auto key = verifiers.get_sign_key(conf.verifier);

			XMAX_ASSERT(conf.is_signer_valid(key), confirmation_validate_exception, "confirmation fail.");
		}

		confirmations.emplace_back(conf);
	}

	bool block_raw::enough_confirmation() const
	{
		int minconf = verifiers.number() * 2 / 3;
		return confirmations.size() >= minconf;
	}
}
}