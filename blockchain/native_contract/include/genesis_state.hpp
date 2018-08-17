/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <blockchain_setup.hpp>
#include <blockchain_types.hpp>

#include <fc/crypto/sha256.hpp>

#include <string>
#include <vector>

namespace Xmaxplatform { namespace Native_contract {
using std::string;
using std::vector;
using Chain::public_key;
using Chain::asset;
using Chain::time;
using Chain::blockchain_setup;

struct genesis_state_type {
   struct initial_account_type {
      initial_account_type(const string& name = string(),
                           uint64_t staking_bal = 1,
                           uint64_t liquid_bal = 0,
                           const public_key& owner_key = public_key(),
                           const public_key& active_key = public_key())
         : name(name), staking_balance(staking_bal), main_token(liquid_bal),
           owner_key(owner_key),
           active_key(active_key == public_key()? owner_key : active_key)
      {}
      string          name;
      asset           staking_balance;
      asset           main_token;
      public_key owner_key;
      public_key active_key;
   };
   struct initial_builder_type {
       initial_builder_type(const string& name = string(),
                            const public_key& signing_key = public_key())
         : owner_name(name), block_signing_key(signing_key)
      {}
      /// Must correspond to one of the initial accounts
      string owner_name;
      public_key block_signing_key;
   };

   time                                     initial_timestamp;
   blockchain_setup                 initial_configuration = {
           Config::default_max_block_size,
           Config::default_target_block_size,
           Config::default_max_storage_size,
           Config::default_elected_pay,
           Config::default_runner_up_pay,
           Config::default_min_xmx_balance,
           Config::default_max_trx_lifetime,
           Config::default_auth_depth_limit,
           Config::default_max_trx_runtime,
           Config::default_inline_depth_limit,
           Config::default_max_inline_msg_size,
           Config::default_max_gen_trx_size
   };
   vector<initial_account_type>             initial_accounts;
   vector<initial_builder_type>            initial_builders;

   initial_account_type xmx_account;
   initial_builder_type xmx_builder;


   /**
    * Temporary, will be moved elsewhere.
    */
   Chain::chain_id_type initial_chain_id;

   /**
    * Get the chain_id corresponding to this genesis state.
    *
    * This is the SHA256 serialization of the genesis_state.
    */
   Chain::chain_id_type compute_chain_id() const;
};

} } // namespace Xmaxplatform::Native_contract

FC_REFLECT(Xmaxplatform::Native_contract::genesis_state_type::initial_account_type,
           (name)(staking_balance)(main_token)(owner_key)(active_key))

FC_REFLECT(Xmaxplatform::Native_contract::genesis_state_type::initial_builder_type, (owner_name)(block_signing_key))

FC_REFLECT(Xmaxplatform::Native_contract::genesis_state_type,
           (initial_timestamp)(initial_configuration)(initial_accounts)
           (initial_builders)(initial_chain_id))
