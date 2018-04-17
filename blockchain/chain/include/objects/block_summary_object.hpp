/**
*  @file
*  @copyright defined in xmax/LICENSE.txt
*/
#pragma once
#include <blockchain_types.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform {
	namespace Chain {
		/**
		*  @brief tracks minimal information about past blocks to implement TaPOS
		*  @ingroup object
		*
		*  When attempting to calculate the validity of a transaction we need to
		*  lookup a past block and check its block hash and the time it occurred
		*  so we can calculate whether the current transaction is valid and at
		*  what time it should expire.
		*/
		class block_summary_object : public Basechain::object<block_summary_object_type, block_summary_object>
		{
			OBJECT_CCTOR(block_summary_object)

				id_type        id;
			xmax_type_block_id  block_id;
		};

		struct by_block_id;
		using block_summary_multi_index = Basechain::shared_multi_index_container<
			block_summary_object,
			indexed_by<
			ordered_unique<tag<by_id>, BOOST_MULTI_INDEX_MEMBER(block_summary_object, block_summary_object::id_type, id)>
			//      ordered_unique<tag<by_block_id>, BOOST_MULTI_INDEX_MEMBER(block_summary_object, xmax_type_block_id, block_id)>
			>
		>;

	}
}

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::block_summary_object, Xmaxplatform::Chain::block_summary_multi_index)

FC_REFLECT(Xmaxplatform::Chain::block_summary_object, (block_id))
