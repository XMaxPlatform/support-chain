/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <block_pack.hpp>

namespace Xmaxplatform {
namespace Chain {

	void block_raw::push_confirm(const block_confirmation& conf)
	{
		for (const auto& item : confirmations)
		{
			FC_ASSERT(item.verifier != conf.verifier, "confirmation had exist.");
		}


		confirmations.emplace_back(conf);
	}

}
}