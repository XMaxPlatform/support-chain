/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <block.hpp>

namespace Xmaxplatform {
namespace Chain {

	struct block_header_summary
	{
		uint32_t			block_num = 0;
		xmax_type_block_id	block_id;
		signed_block_header header;


		const xmax_type_block_id& prev_id() const
		{
			return header.previous;
		}
	};

	struct block_pack : public block_header_summary
	{
		block_pack()
			: block(std::make_shared<signed_block>())
		{
		}



		signed_block_ptr	block;
	};

	using block_pack_ptr = std::shared_ptr<block_pack>;
}
}

FC_REFLECT(Xmaxplatform::Chain::block_header_summary,
	(block_id)(header))

FC_REFLECT_DERIVED(Xmaxplatform::Chain::block_pack, (Xmaxplatform::Chain::block_header_summary), (block))

