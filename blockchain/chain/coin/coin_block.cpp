/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <coin/coin_block.hpp>
#include <chain_utils.hpp>


namespace Xmaxplatform {
namespace Chain {
	xmax_type_summary coin_block_header::digest()const
	{
		return xmax_type_summary::hash(*this);
	}

	xmax_type_block_num coin_block_header::block_num() const
	{
		return utils::num_from_id(previous) + 1;
	}
}
}