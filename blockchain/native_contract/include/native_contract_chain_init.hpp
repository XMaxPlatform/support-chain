/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <genesis_state.hpp>

#include <message.hpp>


namespace Xmaxplatform { namespace Native_contract {

class native_contract_chain_init : public Chain::chain_init {
   genesis_state_type genesis;
public:
    native_contract_chain_init(const genesis_state_type& genesis) : genesis(genesis) {}
   virtual ~native_contract_chain_init() {}

   virtual Basetypes::time get_chain_start_time() override;
   virtual Chain::blockchain_setup get_chain_start_configuration() override;
   virtual std::array<Basetypes::account_name, Config::blocks_per_round> get_chain_start_producers() override;

   virtual void register_types(Chain::chain_xmax& chain, Basechain::database& db) override;
   virtual std::vector<Chain::message> prepare_database(Chain::chain_xmax& chain,
                                                        Basechain::database& db) override;

   static Basetypes::abi xmax_contract_abi();
};

} } // namespace Xmaxplatform::Native_contract

