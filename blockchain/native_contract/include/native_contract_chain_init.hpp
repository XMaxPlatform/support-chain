/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <genesis_state.hpp>
#include <chain_init.hpp>
namespace Xmaxplatform { namespace Chain{
class chain_xmax;
}
}

namespace Xmaxplatform { namespace Native_contract {

class native_contract_chain_init : public Chain::chain_init {
   genesis_state_type genesis;
public:
    native_contract_chain_init(const genesis_state_type& genesis) : genesis(genesis) {}
   virtual ~native_contract_chain_init() {}

   virtual Basetypes::time get_chain_init_time() const override;
   virtual Chain::blockchain_setup get_blockchain_setup() const override;
   virtual Chain::xmax_builder_infos get_chain_init_builders() const override;

   virtual void register_handlers(Chain::chain_xmax &chain, Basechain::database &db) override;
   virtual std::vector<Chain::message_xmax> prepare_data(Chain::chain_xmax &chain,
                                                         Basechain::database &db) override;

   static Basetypes::abi xmax_contract_abi();
};

} } // namespace Xmaxplatform::Native_contract

