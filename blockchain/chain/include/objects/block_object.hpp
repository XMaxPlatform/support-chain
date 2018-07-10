#pragma once
#include <fc/io/raw.hpp>


#include <fc/uint128.hpp>
#include <block.hpp>

#include <boost/multi_index/mem_fun.hpp>

#include "multi_index_includes.hpp"


namespace Xmaxplatform {
	namespace Chain {
		using boost::multi_index_container;
		using namespace boost::multi_index;
		

		class block_object : public Basechain::object<block_object_type, block_object>
		{
			OBJECT_CCTOR(block_object)

			id_type             id;

			xmax_type_block_id blk_id;
			signed_block block;

			Chain::xmax_type_block_num block_num() const { return block.block_num(); }
		};

		
		struct by_blk_id;
		struct by_blk_num;
		using block_multi_index = Basechain::shared_multi_index_container<
			block_object,
			indexed_by<
			ordered_unique<tag<by_id>, BOOST_MULTI_INDEX_MEMBER(block_object, block_object::id_type, id)>,
			ordered_unique<tag<by_blk_id>, BOOST_MULTI_INDEX_MEMBER(block_object, xmax_type_block_id, blk_id)>,
			ordered_non_unique<tag<by_blk_num>, const_mem_fun<block_object, Chain::xmax_type_block_num, &block_object::block_num>>
			>
		>;

		typedef Basechain::generic_index<block_multi_index> block_index;
	}
}

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::block_object, Xmaxplatform::Chain::block_multi_index)

FC_REFLECT(Xmaxplatform::Chain::block_object, (blk_id)(block))
