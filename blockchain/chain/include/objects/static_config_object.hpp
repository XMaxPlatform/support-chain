/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <fc/uint128.hpp>
#include <fc/array.hpp>

#include <blockchain_types.hpp>

#include <basechain.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform { namespace Chain {

   class static_config_object : public Basechain::object<static_config_object_type, static_config_object>
   {
      OBJECT_CCTOR(static_config_object)

      id_type id;
      std::array<account_name, Config::blocks_per_round> active_builders;
   };

   using static_config_multi_index = Basechain::shared_multi_index_container<
           static_config_object,
      indexed_by<
         ordered_unique<tag<by_id>,
            BOOST_MULTI_INDEX_MEMBER(static_config_object, static_config_object::id_type, id)
         >
      >
   >;

}}

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::static_config_object, Xmaxplatform::Chain::static_config_multi_index)

FC_REFLECT(Xmaxplatform::Chain::static_config_object,
           (active_builders)
          )
