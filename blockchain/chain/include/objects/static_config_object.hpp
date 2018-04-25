/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <fc/uint128.hpp>
#include <fc/array.hpp>

#include <blockchain_types.hpp>
#include <builder_rule.hpp>

#include <basechain.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform { namespace Chain {

   class static_config_object : public Basechain::object<static_config_object_type, static_config_object>
   {
      OBJECT_CCTOR(static_config_object, (current_builders)(next_builders)(new_builders))

       id_type id;
       blockchain_setup setup;
	   shared_builder_rule current_builders;

	   shared_builder_rule next_builders;

	   shared_builder_rule new_builders;

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
			(setup)
			(current_builders)
			(next_builders)
			(new_builders)
          )
