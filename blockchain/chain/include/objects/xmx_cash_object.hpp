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

	class xmx_cash_object : public Basechain::object<xmx_cash_object_type, xmx_cash_object> {
		OBJECT_CCTOR(xmx_cash_object, (inputs)(outputs))
	public:
		id_type             id;

		cash_id				cashid;
		time				create_time;

		mcash_inputs		inputs;
		mcash_outputs		outputs;
		cash_attachment		attachment;
		paytype				pay;
		cash_signature		sig;

	};

	struct by_name;
	using xmx_cash_index = Basechain::shared_multi_index_container<
		xmx_cash_object,
		indexed_by<
		ordered_unique<tag<by_id>, member<xmx_cash_object, xmx_cash_object::id_type, &xmx_cash_object::id>>,
		>
	>;

}
} // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::xmx_cash_object, Xmaxplatform::Chain::xmx_cash_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::xmx_cash_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::xmx_cash_object, (cashid)(create_time)(inputs)(outputs)(attachment)(pay)(sig))