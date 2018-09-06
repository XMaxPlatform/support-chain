/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <cash/cash_detail.hpp>
#include "multi_index_includes.hpp"

namespace Xmaxplatform {
namespace Chain {

	using mcash_inputs = mapped_vector<cash_input>;
	using mcash_outputs = mapped_vector<cash_output>;

	class linked_cash_object : public Basechain::object<linked_cash_object_type, linked_cash_object> {
		OBJECT_CCTOR(linked_cash_object,)
	public:
		id_type	id;

		cash_id	fromid;
		uint8_t formslot;

		cash_id toid;
		uint8_t toslot;
	};


	struct by_name;
	using linked_cash_index = Basechain::shared_multi_index_container<
		linked_cash_object,
		indexed_by<
		ordered_unique<tag<by_id>, member<linked_cash_object, linked_cash_object::id_type, &linked_cash_object::id>>
		>
	>;

}
} // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::linked_cash_object, Xmaxplatform::Chain::linked_cash_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::linked_cash_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::linked_cash_object, (fromid)(formslot)(toid)(toslot))