/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <block_pack.hpp>
#include <misc_utilities.hpp>

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
		}

		confirmations.emplace_back(conf);
	}

}
}