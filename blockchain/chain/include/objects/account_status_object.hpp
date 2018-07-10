/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform { namespace Chain {

   class account_status_object : public Basechain::object<account_status_object_type, account_status_object> {
      OBJECT_CCTOR(account_status_object)

      id_type           id;
      account_name		name;

	  uint64_t			trx_counter;
   };

   struct by_name;
   using account_status_index = Basechain::shared_multi_index_container<
	   account_status_object,
	   indexed_by<
	   ordered_unique<tag<by_id>, member<account_status_object, account_status_object::id_type, &account_status_object::id>>,
	   ordered_unique<tag<by_name>, member<account_status_object, account_name, &account_status_object::name>>
	   >
   >;

} } // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::account_status_object, Xmaxplatform::Chain::account_status_index)

FC_REFLECT(Xmaxplatform::Chain::account_status_object::id_type, (_id))

FC_REFLECT(Xmaxplatform::Chain::account_status_object, (id)(name))
