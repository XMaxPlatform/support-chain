/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <include/xmaxtypes.hpp>

namespace Xmaxplatform { namespace Config {
using Basetypes::uint16;
using Basetypes::uint32;
using Basetypes::uint64;
using Basetypes::uint128;
using Basetypes::share_type;
using Basetypes::asset;
using Basetypes::account_name;
using Basetypes::permission_name;

        const static char key_prefix[] = "XMX";
        const static int blocks_per_round = 1;
        const static account_name xmax_contract_name = STN(xmax);

        const static uint32 default_max_block_size = 5 * 1024 * 1024;
        const static uint32 default_target_block_size = 128 * 1024;
        const static uint64 default_max_storage_size = 10 * 1024;
        const static share_type default_elected_pay = asset(100).amount;
        const static share_type default_runner_up_pay = asset(75).amount;
        const static share_type default_min_xmx_balance = asset(100).amount;
        const static uint32 default_max_trx_lifetime = 60*60;
        const static uint16 default_auth_depth_limit = 6;
        const static uint32 default_max_trx_runtime = 10*1000;
        const static uint16 default_inline_depth_limit = 4;
        const static uint32 default_max_inline_msg_size = 4 * 1024;
        const static uint32 default_max_gen_trx_size = 64 * 1024;

} } // namespace Xmaxplatform::Config
