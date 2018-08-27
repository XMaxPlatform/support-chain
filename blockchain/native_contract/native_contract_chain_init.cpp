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
#define SET_SYSTEM_ABI( abi, scope, func, nspace ) \
    abi.actions.push_back( Types::action{name( #func ), #func } );\
	abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes:: ##func## >::type())

#define SET_ERC_ABI( abi, scope, func, nspace ) \
    abi.actions.push_back( Types::action{name( #func ), (std::string(#func) + std::string(#scope)).c_str() } );\
	abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes:: ##func## scope >::type())


#define REGIST_SYSTEM_HANDLER( abi, scope, func, nspace ) \
	chain.set_native_handler(native_scope::native_##scope, #func, & Xmaxplatform::Native_contract::xmax_## scope ##_## func);\
	SET_SYSTEM_ABI( abi, scope, func, nspace )

#define REGIST_ERC_HANDLER( abi, scope, func, nspace ) \
	chain.set_native_handler(native_scope::native_##scope, #func, & Xmaxplatform::Native_contract::xmax_## scope ##_## func);\
	SET_ERC_ABI(abi, scope, func, nspace)



#define SYSTEM_CONTRACT_API(UNPACK_MACRO, _abi) {\
_abi.types.push_back(Types::type_def{ "share_type","int64" }); \
UNPACK_MACRO(_abi, system, addaccount); \
UNPACK_MACRO(_abi, system, addcontract); \
UNPACK_MACRO(_abi, system, adderc20); \
UNPACK_MACRO(_abi, system, adderc721); \
UNPACK_MACRO(_abi, system, transfer); \
UNPACK_MACRO(_abi, system, lock); \
UNPACK_MACRO(_abi, system, unlock); \
UNPACK_MACRO(_abi, system, votebuilder); \
UNPACK_MACRO(_abi, system, regbuilder); \
UNPACK_MACRO(_abi, system, unregbuilder); \
UNPACK_MACRO(_abi, system, regproxy); \
UNPACK_MACRO(_abi, system, unregproxy); \
UNPACK_MACRO(_abi, system, setcode);}

#define ERC20_CONTRACT_API(UNPACK_MACRO, _abi){ \
_abi.types.push_back(Types::type_def{ "share_type","int64" }); \
UNPACK_MACRO(_abi, erc20, mint); \
UNPACK_MACRO(_abi, erc20, stopmint); \
UNPACK_MACRO(_abi, erc20, revoke); \
UNPACK_MACRO(_abi, erc20, transferfrom); }

#define ERC721_CONTRACT_API(UNPACK_MACRO, _abi){\
_abi.types.push_back(Types::type_def{ "share_type","int64" });\
UNPACK_MACRO(_abi, erc721, mint);\
UNPACK_MACRO(_abi, erc721, stopmint);\
UNPACK_MACRO(_abi, erc721, transferfrom);\
UNPACK_MACRO(_abi, erc721, revoke);}


void native_contract_chain_init::register_handlers(chain_xmax &chain, Basechain::database &db) {

	Basetypes::abi sys_abi;

	SYSTEM_CONTRACT_API(REGIST_SYSTEM_HANDLER, sys_abi);

#ifdef USE_V8
	chain.set_native_handler(native_scope::native_system, "setcode", &Xmaxplatform::Native_contract::xmax_system_setjscode);
#endif

	Basetypes::abi erc20_abi;
	ERC20_CONTRACT_API(REGIST_ERC_HANDLER, erc20_abi);

	Basetypes::abi erc721_abi;
	ERC721_CONTRACT_API(REGIST_ERC_HANDLER, erc721_abi);

	chain.set_native_abi(native_scope::native_system, std::move(sys_abi));
	chain.set_native_abi(native_scope::native_erc20, std::move(erc20_abi));
	chain.set_native_abi(native_scope::native_erc721, std::move(erc721_abi));
}

Basetypes::abi native_contract_chain_init::get_system_abi()
{
	Basetypes::abi sys_abi;

	SYSTEM_CONTRACT_API(SET_SYSTEM_ABI, sys_abi);

	return sys_abi;
}

Basetypes::abi native_contract_chain_init::get_erc20_abi()
{
	Basetypes::abi erc20_abi;
	ERC20_CONTRACT_API(SET_ERC_ABI, erc20_abi);
	return erc20_abi;		
}

Basetypes::abi native_contract_chain_init::get_erc721_abi()
{
	Basetypes::abi erc721_abi;
	ERC721_CONTRACT_API(SET_ERC_ABI, erc721_abi);
	return erc721_abi;
}

//        Basetypes::abi native_contract_chain_init::xmax_contract_abi()
//{
//   Basetypes::abi xmax_abi;
//    
//   xmax_abi.types.push_back( Types::type_def{"share_type","int64"} );
//
//    xmax_abi.actions.push_back( Types::action{name("transfer"), "transfer"} );
//    xmax_abi.actions.push_back( Types::action{name("addaccount"), "addaccount"} );
//	xmax_abi.actions.push_back( Types::action{name("setcode"), "setcode" });   
//	xmax_abi.actions.push_back(Types::action{ name("lock"), "lock" });
//	xmax_abi.actions.push_back(Types::action{ name("unlock"), "unlock" });
//	xmax_abi.actions.push_back(Types::action{ name("votebuilder"), "votebuilder" });
//	xmax_abi.actions.push_back(Types::action{ name("regbuilder"), "regbuilder" });
//	xmax_abi.actions.push_back(Types::action{ name("unregbuilder"), "unregbuilder" });
//	xmax_abi.actions.push_back(Types::action{ name("regproxy"), "regproxy" });
//	xmax_abi.actions.push_back(Types::action{ name("unregproxy"), "unregproxy" });
//
//
//	xmax_abi.actions.push_back(Types::action{ name("issueerc20"), "issueerc20" });
//	xmax_abi.actions.push_back(Types::action{ name("minterc20"), "minterc20" });
//	xmax_abi.actions.push_back(Types::action{ name("revokeerc20"), "revokeerc20" });
//	xmax_abi.actions.push_back(Types::action{ name("transfererc20"), "transfererc20" });
//	xmax_abi.actions.push_back(Types::action{ name("transferfromerc20"), "transferfromerc20" });
//	xmax_abi.actions.push_back(Types::action{ name("issueerc721"), "issueerc721" });
//	xmax_abi.actions.push_back(Types::action{ name("minterc721"), "minterc721" });
//	xmax_abi.actions.push_back(Types::action{ name("revokeerc721"), "revokeerc721" });
//	xmax_abi.actions.push_back(Types::action{ name("revoketoken"), "revoketoken" });
//
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::transfer>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::addaccount>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::setcode>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::lock>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unlock>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::votebuilder>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::regbuilder>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unregbuilder>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::regproxy>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unregproxy>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::issueerc20>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::minterc20>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::revokeerc20>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::transfererc20>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::transferfromerc20>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::issueerc721>::type());
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::minterc721>::type());	
//	xmax_abi.structs.push_back(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::revokeerc721>::type());
//
//
//   return xmax_abi;
//}

static void create_native_account(Basechain::database &db, account_name name, share_type main_token, time creation_date) {

	db.create<account_object>([&](account_object& a) {
		a.name = name;
		a.type = account_type::acc_system;
		a.creation_date = creation_date;
	});

	db.create<Xmaxplatform::Chain::xmx_token_object>([&name, main_token](auto& b) {
		b.owner_name = name;
		b.main_token = main_token;
	});
};

std::vector<Chain::message_data> native_contract_chain_init::prepare_data(chain_xmax &chain,
                                                                   Basechain::database &db) {
	std::vector<Chain::message_data> messages_to_process;

   create_native_account(db, Config::xmax_contract_name, Config::initial_token_supply, genesis.initial_timestamp);

   // Queue up messages which will run contracts to create the initial accounts
   auto KeyAuthority = [](public_key k) {
      return Basetypes::authority(1, {{k, 1}}, {});
   };
   for (const auto& acct : genesis.initial_accounts) {

	   account_name acc_name = acct.name;
	   message_data data;
	   data.msg = message_xmax(Config::xmax_contract_name,
		   vector<Basetypes::account_auth>{ {Config::xmax_contract_name, Config::xmax_active_name}},
		   "addaccount", Basetypes::addaccount(Config::xmax_contract_name, acc_name,
			   KeyAuthority(acct.owner_key),
			   KeyAuthority(acct.active_key),
			   acct.staking_balance));
	   messages_to_process.emplace_back(std::move(data));



	   if (acct.main_token > 0) {
		   message_data data2;

		   data2.msg = message_xmax(Config::xmax_contract_name,
			   vector<Basetypes::account_auth>{ {Config::xmax_contract_name, Config::xmax_active_name}},
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
