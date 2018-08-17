#pragma once
#include <fc/io/raw.hpp>
#include <fc/uint128.hpp>

#include <basechain.hpp>
#include <transaction.hpp>

#include <boost/multi_index/mem_fun.hpp>
#include "multi_index_includes.hpp"







namespace Xmaxplatform {
	namespace Chain {
		using boost::multi_index_container;
		using namespace boost::multi_index;
		

		class transaction_object : public Basechain::object<transaction_object_type, transaction_object>
		{
			OBJECT_CCTOR(transaction_object)

			id_type             id;
			time_point_sec      expiration;
			xmax_type_transaction_id trx_id;

			time_point_sec get_expiration()const { return expiration; }
		};

		struct by_expiration;
		struct by_trx_id;
		using transaction_multi_index = Basechain::shared_multi_index_container<
			transaction_object,
			indexed_by<
			ordered_unique<tag<by_id>, BOOST_MULTI_INDEX_MEMBER(transaction_object, transaction_object::id_type, id)>,
			ordered_unique<tag<by_trx_id>, BOOST_MULTI_INDEX_MEMBER(transaction_object, xmax_type_transaction_id, trx_id)>,
			ordered_non_unique<tag<by_expiration>, const_mem_fun<transaction_object, time_point_sec, &transaction_object::get_expiration>>
			>
		>;

		typedef Basechain::generic_index<transaction_multi_index> transaction_index;
	}
}

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::transaction_object, Xmaxplatform::Chain::transaction_multi_index)

FC_REFLECT(Xmaxplatform::Chain::transaction_object, (expiration)(trx_id))
