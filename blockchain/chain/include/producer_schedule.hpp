/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <blockchain_types.hpp>
#include <vector>


namespace Xmaxplatform { namespace Chain {

    struct producer_key
    {
        account_name        producer_name;
        public_key_type     block_signing_key;
    };

    struct producer_schedule
    {
    public:
        uint32_t                     version;   ///< sequentially incrementing version number
        std::vector<producer_key>    producers;
    };



}}
