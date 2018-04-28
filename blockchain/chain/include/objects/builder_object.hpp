/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform {
namespace Chain {

	class builder_object : public Basechain::object<builder_object_type, builder_object> {
		OBJECT_CCTOR(builder_object)
	public:
		id_type             id;
		account_name		owner;
		public_key_type		signing_key;
		time				last_block_time;
		int64_t				total_missed = 0;
	};

	struct by_key;
	struct by_owner;
	using builder_multi_index = Basechain::shared_multi_index_container <
		builder_object,
		indexed_by<
		ordered_unique<tag<by_id>, member<builder_object, builder_object::id_type, &builder_object::id>>,
		ordered_unique<tag<by_owner>, member<builder_object, account_name, &builder_object::owner>>,
		ordered_unique<tag<by_key>,
		composite_key<builder_object,
		member<builder_object, public_key_type, &builder_object::signing_key>,
		member<builder_object, builder_object::id_type, &builder_object::id>
		>
		>
		>
	>;
}
}
BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::builder_object, Xmaxplatform::Chain::builder_multi_index)

FC_REFLECT(Xmaxplatform::Chain::builder_object::id_type, (_id))
FC_REFLECT(Xmaxplatform::Chain::builder_object, (id)(owner)(signing_key)(last_block_time)(total_missed) )
