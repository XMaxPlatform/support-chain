/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <blockchain_types.hpp>
#include "multi_index_includes.hpp"
#include <basechain.hpp>

namespace Xmaxplatform {
namespace Chain {


/**
 * @brief The balance_object class tracks the XMX balance for accounts
 */
class xmx_token_object : public Basechain::object<xmx_token_object_type, xmx_token_object> {
   OBJECT_CCTOR(xmx_token_object)

   id_type id;
    Basetypes::account_name owner_name;
    Basetypes::share_type xmx_token = 0;
};

struct by_owner_name;

using xmx_token_multi_index = Basechain::shared_multi_index_container<
        xmx_token_object,
   indexed_by<
      ordered_unique<tag<by_id>,
         member<xmx_token_object, xmx_token_object::id_type, &xmx_token_object::id>
      >,
      ordered_unique<tag<by_owner_name>,
         member<xmx_token_object, Basetypes::account_name, &xmx_token_object::owner_name>
      >
   >
>;

} } // namespace Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::xmx_token_object, Xmaxplatform::Chain::xmx_token_multi_index)
