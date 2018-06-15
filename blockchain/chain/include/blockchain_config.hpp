/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <include/xmaxtypes.hpp>
#include <include/basemisc.hpp>
#include <chain_time.hpp>
#include <publickey.hpp>
#include <fc/crypto/elliptic.hpp>



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
using fc::ecc::private_key;
        const static char key_prefix[] = "XMX";
        const static int blocks_per_round = 21;
		const static int builders_per_round = 21;
        const static account_name xmax_contract_name = STN(xmax);

		const extern public_key xmax_build_public_key;
		const extern private_key xmax_build_private_key;

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

		const static uint64_t chain_timestamp_epoch_s = 946684800; // year 2000 AD.

        const static uint64_t chain_timestamp_epoch_us = chain_timestamp_epoch_s * 1000ll * 1000ll; 



		enum validation_steps
		{
			skip_nothing = 0,
			skip_producer_signature = 1 << 0,  ///< used while reindexing
			skip_transaction_signatures = 1 << 1,  ///< used by non-builder nodes
			skip_transaction_dupe_check = 1 << 2,  ///< used while reindexing
			skip_fork_db = 1 << 3,  ///< used while reindexing
			skip_block_size_check = 1 << 4,  ///< used when applying locally generated transactions
			skip_tapos_check = 1 << 5,  ///< used while reindexing -- note this skips expiration check as well
			skip_authority_check = 1 << 6,  ///< used while reindexing -- disables any checking of authority on transactions
			skip_merkle_check = 1 << 7,  ///< used while reindexing
			skip_assert_evaluation = 1 << 8,  ///< used while reindexing
			skip_undo_history_check = 1 << 9,  ///< used while reindexing
			skip_builder_rule_check = 1 << 10, ///< used while reindexing
			skip_validate = 1 << 11, ///< used prior to checkpoint, skips validate() call on transaction
			skip_scope_check = 1 << 12, ///< used to skip checks for proper scope
			skip_output_check = 1 << 13, ///< used to skip checks for outputs in block exactly matching those created from apply
			pushed_transaction = 1 << 14, ///< used to indicate that the origination of the call was from a push_transaction, to determine time allotment
			created_block = 1 << 15, ///< used to indicate that the origination of the call was for creating a block, to determine time allotment
			received_block = 1 << 16, ///< used to indicate that the origination of the call was for a received block, to determine time allotment
			irreversible = 1 << 17,  ///< indicates the block was received while catching up and is already considered irreversible.
			skip_confirmation = 1 << 18  ///< skip confirmation.
		};

} } // namespace Xmaxplatform::Config
