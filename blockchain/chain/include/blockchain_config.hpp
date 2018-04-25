/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <include/xmaxtypes.hpp>
#include <include/basemisc.hpp>
#include <chain_time.hpp>

namespace Xmaxplatform { namespace Config {
using Basetypes::uint16;
using Basetypes::uint32;
using Basetypes::uint64;
using Basetypes::uint128;
using Basetypes::share_type;
using Basetypes::asset;
using Basetypes::account_name;
using Basetypes::permission_name;
using Basetypes::public_key;
        const static char key_prefix[] = "XMX";
        const static int blocks_per_round = 21;
        const static account_name xmax_contract_name = STN(xmax);
		const static public_key xmax_contract_key = empty_public_key;

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

        const static share_type initial_token_supply = asset::from_string("1000000000.0000 XMX").amount;

        const static int default_per_code_account_max_db_limit_mbytes = 5;
        const static int default_row_overhead_db_limit_bytes = 8 + 8 + 8 + 8; // storage for scope/code/table + 8 extra


        const static uint32_t chain_timestamp_unit_ms = 1000;
        const static uint32_t chain_timestamp_unit_us = chain_timestamp_unit_ms * 1000;
		const static uint32_t mini_next_block_us = chain_timestamp_unit_us / 10;

        const static uint64_t chain_timestamp_epoch_us = 946684800000ll; // year 2000 AD.

} } // namespace Xmaxplatform::Config
