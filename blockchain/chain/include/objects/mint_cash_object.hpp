/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <cash/cash_mint.hpp>
#include "multi_index_includes.hpp"

namespace Xmaxplatform {
namespace Chain {


	class mint_cash_object : public Basechain::object<mint_cash_object_type, mint_cash_object> {
		OBJECT_CCTOR(mint_cash_object,)
	public:
		id_type	id;

		cash_mint mint;

		int64_t get_sequence() const
		{
			return mint.sequence;
		}

	};


	struct by_sequence;
	using mint_cash_index = Basechain::shared_multi_index_container<
		mint_cash_object,
		indexed_by<
		ordered_unique<tag<by_id>, member<mint_cash_object, mint_cash_object::id_type, &mint_cash_object::id>>,
		ordered_unique<tag<by_sequence>, const_mem_fun<mint_cash_object, int64_t, &mint_cash_object::get_sequence>>
		>
	>;

}
} // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::mint_cash_object, Xmaxplatform::Chain::mint_cash_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::mint_cash_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::mint_cash_object, (fromid)(formslot)(toid)(toslot))