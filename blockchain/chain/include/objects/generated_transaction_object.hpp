#pragma once
#include <fc/io/raw.hpp>

#include <transaction.hpp>
#include <fc/uint128.hpp>

#include <boost/multi_index/mem_fun.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform {
	namespace Chain {
		using boost::multi_index_container;
		using namespace boost::multi_index;
	
		class generated_transaction_object : public Basechain::object<generated_transaction_object_type, generated_transaction_object>
		{
			OBJECT_CCTOR(generated_transaction_object)

				enum status_type {
				PENDING = 0,
				PROCESSED
			};


			id_type                       id;
			generated_transaction          trx;
			status_type                   status;

			time_point_sec get_expiration()const { return trx.expiration; }
			xmax_type_generated_transaction_id get_id() const { return trx.id; }

			struct by_trx_id;
			struct by_expiration;
			struct by_status;
		};

		using generated_transaction_multi_index = Basechain::shared_multi_index_container<
			generated_transaction_object,
			indexed_by<
			ordered_unique<tag<by_id>, BOOST_MULTI_INDEX_MEMBER(generated_transaction_object, generated_transaction_object::id_type, id)>,
			ordered_unique<tag<generated_transaction_object::by_trx_id>, const_mem_fun<generated_transaction_object, xmax_type_generated_transaction_id, &generated_transaction_object::get_id>>,
			ordered_non_unique<tag<generated_transaction_object::by_expiration>, const_mem_fun<generated_transaction_object, time_point_sec, &generated_transaction_object::get_expiration>>,
			ordered_non_unique<tag<generated_transaction_object::by_status>, BOOST_MULTI_INDEX_MEMBER(generated_transaction_object, generated_transaction_object::status_type, status)>
			>
		>;

		typedef Basechain::generic_index<generated_transaction_multi_index> generated_transaction_index;
	}
}

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::generated_transaction_object, Xmaxplatform::Chain::generated_transaction_multi_index)

FC_REFLECT(Xmaxplatform::Chain::generated_transaction_object, (trx))
