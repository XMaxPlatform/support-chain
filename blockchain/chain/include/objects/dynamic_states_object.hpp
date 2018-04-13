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


   class dynamic_states_object : public Basechain::object<dynamic_states_object_type, dynamic_states_object>
   {
        OBJECT_CCTOR(dynamic_states_object)

        id_type                 id;
        uint32_t                head_block_number = 0;
        xmax_type_block_id      head_block_id;
        fc::time_point          state_time;
        account_name            current_builder;

   };

   using dynamic_states_multi_index = Basechain::shared_multi_index_container<
           dynamic_states_object,
      indexed_by<
         ordered_unique<tag<by_id>,
            BOOST_MULTI_INDEX_MEMBER(dynamic_states_object, dynamic_states_object::id_type, id)
         >
      >
   >;

}}


BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::dynamic_states_object,
                         Xmaxplatform::Chain::dynamic_states_multi_index)

FC_REFLECT(Xmaxplatform::Chain::dynamic_states_object,
           (head_block_number)
           (head_block_id)
           (state_time)
           (current_builder)
          )
