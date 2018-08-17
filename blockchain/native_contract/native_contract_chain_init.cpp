/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <native_contract_chain_init.hpp>
#include <blockchain_exceptions.hpp>
#include <chain_xmax.hpp>
#include <xmax_contract.hpp>
#include <objects/contract_object.hpp>
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
	SET_APP_HANDLER(xmax, xmax, issueerc20, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, minterc20, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, revokeerc20, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, transfererc20, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, transferfromerc20, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, issueerc721, Xmaxplatform);
	SET_APP_HANDLER(xmax, xmax, minterc721, Xmaxplatform);	
	SET_APP_HANDLER(xmax, xmax, revokeerc721, Xmaxplatform);

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
	xmax_abi.actions.push_back(Types::action{ name("issueerc20"), "issueerc20" });
	xmax_abi.actions.push_back(Types::action{ name("minterc20"), "minterc20" });
	xmax_abi.actions.push_back(Types::action{ name("revokeerc20"), "revokeerc20" });
	xmax_abi.actions.push_back(Types::action{ name("transfererc20"), "transfererc20" });
	xmax_abi.actions.push_back(Types::action{ name("transferfromerc20"), "transferfromerc20" });
	xmax_abi.actions.push_back(Types::action{ name("issueerc721"), "issueerc721" });
	xmax_abi.actions.push_back(Types::action{ name("minterc721"), "minterc721" });
	xmax_abi.actions.push_back(Types::action{ name("revokeerc721"), "revokeerc721" });
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
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::issueerc20>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::minterc20>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::revokeerc20>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::transfererc20>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::transferfromerc20>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::issueerc721>::type());
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::minterc721>::type());	
	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::revokeerc721>::type());


   return xmax_abi;
}

std::vector<Chain::message_data> native_contract_chain_init::prepare_data(chain_xmax &chain,
                                                                   Basechain::database &db) {
	std::vector<Chain::message_data> messages_to_process;


   auto CreateNativeAccount = [this, &db](name name, auto main_token) {

       db.create<account_object>([this, &name](account_object& a) {
           a.name = name;
		   a.type = account_type::acc_system;
           a.creation_date = genesis.initial_timestamp;
       });
	   if (name == Config::xmax_contract_name) {
		   db.create<contract_object>([this, &name](contract_object& a) {
			   a.name = name;
			   a.set_abi(xmax_contract_abi());	   
		   });
	   }


      db.create<Xmaxplatform::Chain::xmx_token_object>([&name, main_token]( auto& b) {
         b.owner_name = name;
         b.main_token = main_token;
      });
   };
   CreateNativeAccount(Config::xmax_contract_name, Config::initial_token_supply);

   // Queue up messages which will run contracts to create the initial accounts
   auto KeyAuthority = [](public_key k) {
      return Basetypes::authority(1, {{k, 1}}, {});
   };
   for (const auto& acct : genesis.initial_accounts) {

	   account_name acc_name = acct.name;
	   message_data data;
	   data.msg = message_xmax(Config::xmax_contract_name,
		   vector<Basetypes::account_auth>{ {acc_name, Config::xmax_active_name}},
		   "addaccount", Basetypes::addaccount(Config::xmax_contract_name, acc_name,
			   KeyAuthority(acct.owner_key),
			   KeyAuthority(acct.active_key),
			   acct.staking_balance));

	   messages_to_process.emplace_back(std::move(data));



	   if (acct.main_token > 0) {
		   message_data data2;

		   data2.msg = message_xmax(Config::xmax_contract_name,
			   vector<Basetypes::account_auth>{ {acc_name, Config::xmax_active_name}},
			   "transfer", Basetypes::transfer(Config::xmax_contract_name, acct.name,
				   acct.main_token.amount, "Genesis Allocation"));

		   data2.scopes.push_back(acc_name);
		   data2.scopes.push_back(Config::xmax_contract_name);

		   messages_to_process.emplace_back(std::move(data2));
	   }
   }

   for (const auto& blder : genesis.initial_builders)
   {
	   message_data data;
	   account_name acc_name = blder.owner_name;
	   data.msg = message_xmax(Config::xmax_contract_name,
		   vector<Basetypes::account_auth>{ {acc_name, Config::xmax_active_name}},
		   "regbuilder", Basetypes::regbuilder(acc_name, blder.block_signing_key));

	   messages_to_process.emplace_back(std::move(data));
   }

   return messages_to_process;
}

} } // namespace Xmaxplatform::Native_contract
