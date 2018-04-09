/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <native_contract_chain_init.hpp>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace Xmaxplatform { namespace Native_contract {
using namespace Xmaxplatform::Chain;

        Basetypes::time native_contract_chain_init::get_chain_init_time() {
   return genesis.initial_timestamp;
}

blockchain_setup native_contract_chain_init::get_blockchain_setup() {
   return genesis.initial_configuration;
}

std::array<Basetypes::account_name, Config::blocks_per_round> native_contract_chain_init::get_chain_init_builders() {
   std::array<Basetypes::account_name, Config::blocks_per_round> result;
   std::transform(genesis.initial_builders.begin(), genesis.initial_builders.end(), result.begin(),
                  [](const auto& p) { return p.owner_name; });
   return result;
}

void native_contract_chain_init::register_types(chain_xmax& chain, Basechain::database& db) {

}
        Basetypes::abi native_contract_chain_init::xmax_contract_abi()
{
   Basetypes::abi xmax_abi;


   return xmax_abi;
}

std::vector<message_xmax> native_contract_chain_init::prepare_database(chain_xmax& chain,
                                                                                Basechain::database& db) {
   std::vector<message_xmax> messages_to_process;


   auto CreateNativeAccount = [this, &db](name name, auto xmxtoken) {

       db.create<account_object>([this, &name](account_object& a) {
           a.name = name;
           a.creation_date = genesis.initial_timestamp;

           if( name == Config::xmax_contract_name ) {
              a.set_abi(xmax_contract_abi());
           }
       });

      db.create<Xmaxplatform::Chain::xmx_token_object>([&name, xmxtoken]( auto& b) {
         b.owner_name = name;
         b.xmx_token = xmxtoken;
      });
   };
   CreateNativeAccount(Config::xmax_contract_name, Config::initial_token_supply);

   // Queue up messages which will run contracts to create the initial accounts
   auto KeyAuthority = [](public_key k) {
      return Basetypes::authority(1, {{k, 1}}, {});
   };
   for (const auto& acct : genesis.initial_accounts) {
      message_xmax msg(Config::xmax_contract_name,
                             vector<Basetypes::account_permission>{{Config::xmax_contract_name, "active"}},
                             "newaccount", Basetypes::newaccount(Config::xmax_contract_name, acct.name,
                                                             KeyAuthority(acct.owner_key),
                                                             KeyAuthority(acct.active_key),
                                                             KeyAuthority(acct.owner_key),
                                                             acct.staking_balance));
      messages_to_process.emplace_back(std::move(msg));
      if (acct.xmx_token > 0) {
         msg = message_xmax(Config::xmax_contract_name,
                                  vector<Basetypes::account_permission>{{Config::xmax_contract_name, "active"}},
                                  "transfer", Basetypes::transfer(Config::xmax_contract_name, acct.name,
                                                              acct.xmx_token.amount, "Genesis Allocation"));
         messages_to_process.emplace_back(std::move(msg));
      }
   }

   return messages_to_process;
}

} } // namespace Xmaxplatform::Native_contract
