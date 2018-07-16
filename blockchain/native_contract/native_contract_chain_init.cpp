/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <native_contract_chain_init.hpp>
#include <blockchain_exceptions.hpp>
#include <chain_xmax.hpp>
#include <xmax_contract.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace Xmaxplatform { namespace Native_contract {
using namespace Xmaxplatform::Chain;

Basetypes::time native_contract_chain_init::get_chain_init_time() const {
   return genesis.initial_timestamp;
}

Chain::blockchain_setup native_contract_chain_init::get_blockchain_setup() const {
   return genesis.initial_configuration;
}

xmax_builder_infos native_contract_chain_init::get_chain_init_builders() const {
	xmax_builder_infos result;
	for (auto it : genesis.initial_builders)
	{
		result.push_back(builder_info(it.owner_name, it.block_signing_key));
	}
	return result;
}

void native_contract_chain_init::register_handlers(chain_xmax &chain, Basechain::database &db) {

#define SET_APP_HANDLER( contract, scope, action, nspace ) \
   chain.set_message_handler( #contract, #scope, #action, &BOOST_PP_CAT(nspace::Native_contract::handle_, BOOST_PP_CAT(contract, BOOST_PP_CAT(_,action) ) ) )
    SET_APP_HANDLER( xmax, xmax, addaccount, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, transfer, Xmaxplatform );
	
    SET_APP_HANDLER( xmax, xmax, lock, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, unlock, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, votebuilder, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, regbuilder, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, unregbuilder, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, regproxy, Xmaxplatform );
    SET_APP_HANDLER( xmax, xmax, unregproxy, Xmaxplatform );
	SET_APP_HANDLER(xmax, xmax, setcode, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, issueerc2o, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, minterc2o, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, issueerc21, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, minterc21, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, revokeerc2o, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, revokeerc21, Xmaxplatform);

#ifdef USE_V8
	SET_APP_HANDLER(xmax, xmax, setjscode, Xmaxplatform);
#endif
}

        Basetypes::abi native_contract_chain_init::xmax_contract_abi()
{
   Basetypes::abi xmax_abi;
    
   xmax_abi.types.push_back( Types::type_def{"share_type","int64"} );

    xmax_abi.actions.push_back( Types::action{name("transfer"), "transfer"} );
    xmax_abi.actions.push_back( Types::action{name("addaccount"), "addaccount"} );
	xmax_abi.actions.push_back( Types::action{name("setcode"), "setcode" });   
	xmax_abi.actions.push_back(Types::action{ name("lock"), "lock" });
	xmax_abi.actions.push_back(Types::action{ name("unlock"), "unlock" });
	xmax_abi.actions.push_back(Types::action{ name("votebuilder"), "votebuilder" });
	xmax_abi.actions.push_back(Types::action{ name("regbuilder"), "regbuilder" });
	xmax_abi.actions.push_back(Types::action{ name("unregbuilder"), "unregbuilder" });
	xmax_abi.actions.push_back(Types::action{ name("regproxy"), "regproxy" });
	xmax_abi.actions.push_back(Types::action{ name("unregproxy"), "unregproxy" });
	xmax_abi.actions.push_back(Types::action{ name("issueerc2o"), "issueerc2o" });
	xmax_abi.actions.push_back(Types::action{ name("issueerc21"), "issueerc21" });
	xmax_abi.actions.push_back(Types::action{ name("revoketoken"), "revoketoken" });

	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::transfer>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::addaccount>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::setcode>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::lock>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unlock>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::votebuilder>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::regbuilder>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unregbuilder>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::regproxy>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unregproxy>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::issueerc2o>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::minterc2o>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::revokeerc2o>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::issueerc21>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::minterc21>::type());	
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::revokeerc21>::type());


   return xmax_abi;
}

std::vector<message_xmax> native_contract_chain_init::prepare_data(chain_xmax &chain,
                                                                   Basechain::database &db) {
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

	   account_name acc_name = acct.name;

	   message_xmax msg1(Config::xmax_contract_name,
		   vector<Basetypes::account_permission>{ {acc_name, "active"}},
		   "addaccount", Basetypes::addaccount(Config::xmax_contract_name, acc_name,
			   KeyAuthority(acct.owner_key),
			   KeyAuthority(acct.active_key),
			   acct.staking_balance));

	   messages_to_process.emplace_back(std::move(msg1));
	   if (acct.xmx_token > 0) {
		   message_xmax msg2(Config::xmax_contract_name,
			   vector<Basetypes::account_permission>{ {acc_name, "active"}},
			   "transfer", Basetypes::transfer(Config::xmax_contract_name, acct.name,
				   acct.xmx_token.amount, "Genesis Allocation"));
		   messages_to_process.emplace_back(std::move(msg2));


	   }
   }

   for (const auto& blder : genesis.initial_builders)
   {
	   account_name acc_name = blder.owner_name;
	   message_xmax msg(Config::xmax_contract_name,
		   vector<Basetypes::account_permission>{ {acc_name, "active"}},
		   "regbuilder", Basetypes::regbuilder(acc_name, blder.block_signing_key));

	   messages_to_process.emplace_back(std::move(msg));
   }

   return messages_to_process;
}

} } // namespace Xmaxplatform::Native_contract
