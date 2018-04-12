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
 * @brief The resource_token_object class
 */
class resource_token_object : public Basechain::object<resource_token_object_type, resource_token_object> {
    OBJECT_CCTOR(resource_token_object)

    id_type         id;
    account_name    owner_name;
    share_type      locked_token = 0;
    share_type      unlocked_token = 0;
    time            last_unlocked_time = time::maximum();
};

    struct by_owner_name;

    using resource_token_multi_index = Basechain::shared_multi_index_container<
            resource_token_object,
            indexed_by<
                    ordered_unique<tag<by_id>,
                            member<resource_token_object, resource_token_object::id_type, &resource_token_object::id>
                    >,
                    ordered_unique<tag<by_owner_name>,
                            member<resource_token_object, account_name, &resource_token_object::owner_name>
                    >
            >
    >;

} } // namespace Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::resource_token_object, Xmaxplatform::Chain::resource_token_multi_index)
