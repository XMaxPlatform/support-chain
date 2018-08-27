/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <coin/coin_types.hpp>


namespace Xmaxplatform {
namespace Chain {

	struct coin_block_header
	{
		xmax_type_summary			digest() const;
		xmax_type_block_num			block_num() const;

		xmax_type_block_id		previous;
		chain_timestamp			timestamp;
		account_address			builder;
	};

}
}


FC_REFLECT(Xmaxplatform::Chain::coin_block_header, (previous)(timestamp)(builder))