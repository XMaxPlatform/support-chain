/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <blockchain_types.hpp>
#include <vector>


namespace Xmaxplatform { namespace Chain {

    struct builder_info
    {
        account_name        builder_name;
        public_key_type     block_signing_key;
    };



    struct builder_schedule
    {
    public:
        typedef std::array<account_name, Config::blocks_per_round> xmax_builders;
        uint32_t            version;   ///< sequentially incrementing version number
        xmax_builders       builders;

        inline builder_schedule()
        {
            version = 0;
        }

        void set_builders(const xmax_builders& list)
        {
            builders = list;
        }
    };



}}
