/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <blockchain_types.hpp>
#include <vector>


namespace Xmaxplatform { namespace Chain {

    struct builder_key
    {
        account_name        builder_name;
        public_key_type     block_signing_key;
    };

    struct builder_schedule
    {
    public:
        uint32_t                     version;   ///< sequentially incrementing version number
        std::vector<builder_key>     builders;
    };



}}
