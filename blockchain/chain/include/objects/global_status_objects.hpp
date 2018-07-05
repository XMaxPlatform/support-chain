/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <blockchain_types.hpp>

#include <basechain.hpp>



namespace Xmaxplatform { namespace Chain {

	class global_trx_status_object : public Basechain::object<global_trx_status_object_type, global_trx_status_object>
	{
		OBJECT_CCTOR(global_trx_status_object)

		id_type id;
		
		uint64_t counter;
	};

	using global_trx_status_index = Basechain::shared_multi_index_container<
		global_trx_status_object,
		indexed_by<
			ordered_unique<tag<by_id>,
			BOOST_MULTI_INDEX_MEMBER(global_trx_status_object, global_trx_status_object::id_type, id)
			>
		>
	>;

	class global_msg_status_object : public Basechain::object<global_msg_status_object_type, global_msg_status_object>
	{
		OBJECT_CCTOR(global_msg_status_object)

			id_type id;

		uint64_t counter;
	};

	using global_msg_status_index = Basechain::shared_multi_index_container<
		global_msg_status_object,
		indexed_by<
		ordered_unique<tag<by_id>,
		BOOST_MULTI_INDEX_MEMBER(global_msg_status_object, global_msg_status_object::id_type, id)
		>
		>
	>;
}}

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::global_trx_status_object, Xmaxplatform::Chain::global_trx_status_index)

FC_REFLECT(Xmaxplatform::Chain::global_trx_status_object,
(counter)
)

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::global_msg_status_object, Xmaxplatform::Chain::global_msg_status_index)

FC_REFLECT(Xmaxplatform::Chain::global_msg_status_object,
(counter)
)
