/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <xmax_contract.hpp>
#include <blockchain_exceptions.hpp>
#include <chain_xmax.hpp>
#include <message_context_xmax.hpp>
#include <message_xmax.hpp>

#include <objects/object_utility.hpp>
#include <objects/account_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/resource_token_object.hpp>
#include <objects/chain_object_table.hpp>
#include <objects/static_config_object.hpp>
#include <objects/erc20_token_object.hpp>
#include <objects/erc721_token_object.hpp>
#include <objects/erc721_token_account_object.hpp>
#include <xmax_voting.hpp>
#include <vm_xmax.hpp>

#include <abi_serializer.hpp>

#ifdef USE_V8
#include <jsvm_xmax.h>
#endif



namespace Xmaxplatform {
    namespace Native_contract {
using namespace Chain;
namespace config = ::Xmaxplatform::Config;
namespace Chain = ::Xmaxplatform::Chain;
using namespace ::Xmaxplatform::Basetypes;

typedef mutable_db_table <xmx_token_object, by_owner_name> xmax_token_table;
typedef mutable_db_table <resource_token_object, by_owner_name> resource_token_table;

//Utility functions
static void create_account_internal(Types::addaccount& create, Basechain::database& db, time& current_time) {
		
	auto existing_account = db.find<account_object, by_name>(create.name);
	XMAX_ASSERT(existing_account == nullptr, account_name_exists_exception,
		"Cannot create account named ${name}, as that name is already taken",
		("name", create.name));

	const auto& new_account = db.create<account_object>([&create, &current_time](account_object& a) {
		a.name = create.name;
		a.creation_date = current_time;
	});

	const auto& creatorToken = db.get<xmx_token_object, by_owner_name>(create.creator);

	XMAX_ASSERT(creatorToken.xmx_token >= create.deposit.amount, message_validate_exception,
		"Creator '${c}' has insufficient funds to make account creation deposit of ${a}",
		("c", create.creator)("a", create.deposit));

	db.modify(creatorToken, [&create](xmx_token_object& b) {
		b.xmx_token -= create.deposit.amount;
	});

	db.create<xmx_token_object>([&create](xmx_token_object& b) {
		b.owner_name = create.name;
		b.xmx_token = 0; //create.deposit.amount; TODO: make sure we credit this in @staked
	});
}



//API handler implementations
void handle_xmax_addaccount(message_context_xmax& context) {	
	create_account_internal(context.msg.as<Types::addaccount>(), context.mutable_db, context.current_time());
}

void handle_xmax_transfer(message_context_xmax& context) {
   auto transfer = context.msg.as<Types::transfer>();

   try {
      XMAX_ASSERT(transfer.amount > 0, message_validate_exception, "Must transfer a positive amount");
      context.require_scope(transfer.to);
      context.require_scope(transfer.from);

      context.require_recipient(transfer.to);
      context.require_recipient(transfer.from);
   } FC_CAPTURE_AND_RETHROW((transfer))


   try {
      auto& db = context.mutable_db;
      const auto& from = db.get<xmx_token_object, by_owner_name>(transfer.from);

      XMAX_ASSERT(from.xmx_token >= transfer.amount, message_precondition_exception, "Insufficient Funds",
                 ("from.xmx_token",from.xmx_token)("transfer.amount",transfer.amount));

      const auto& to = db.get<xmx_token_object, by_owner_name>(transfer.to);
      db.modify(from, [&](xmx_token_object& a) {
         a.xmx_token -= share_type(transfer.amount);
      });
      db.modify(to, [&](xmx_token_object& a) {
         a.xmx_token += share_type(transfer.amount);
      });
   } FC_CAPTURE_AND_RETHROW( (transfer) ) 
}

void handle_xmax_lock(message_context_xmax& context) {
    auto lock = context.msg.as<Types::lock>();

    XMAX_ASSERT(lock.amount > 0, message_validate_exception, "Locked amount must be positive");

    context.require_scope(lock.to);
    context.require_scope(lock.from);
    context.require_scope(config::xmax_contract_name);

    context.require_authorization(lock.from);

    context.require_recipient(lock.to);
    context.require_recipient(lock.from);

    xmax_token_table xmax_token_tbl(context.mutable_db);
    resource_token_table resource_token_tbl(context.mutable_db);

    const xmx_token_object& locker = xmax_token_tbl.get(lock.from);

    XMAX_ASSERT( locker.xmx_token >= lock.amount, message_precondition_exception,
                 "Account ${a} lacks sufficient funds to lock ${amt} XMX", ("a", lock.from)("amt", lock.amount)("available",locker.xmx_token) );

    const auto& resource_token = resource_token_tbl.get(lock.to);

    xmax_token_tbl.modify(locker, [&lock](xmx_token_object& a) {
        a.xmx_token -= share_type(lock.amount);
    });

    resource_token_tbl.modify(resource_token, [&lock](resource_token_object& a){
       a.locked_token += share_type(lock.amount);
    });


    xmax_voting::increase_votes(context, lock.to, share_type(lock.amount));
}

void handle_xmax_unlock(message_context_xmax& context)      {
    auto unlock = context.msg.as<Types::unlock>();

    context.require_authorization(unlock.account);

    XMAX_ASSERT(unlock.amount >= 0, message_validate_exception, "Unlock amount cannot be negative");

    resource_token_table resource_token_tbl(context.mutable_db);

    const auto& unlocker = resource_token_tbl.get(unlock.account);

    XMAX_ASSERT(unlocker.locked_token  >= unlock.amount, message_precondition_exception,
                "Insufficient locked funds to unlock ${a}", ("a", unlock.amount));

    resource_token_tbl.modify(unlocker, [&unlock, &context](resource_token_object& a) {
        a.locked_token -= share_type(unlock.amount);
        a.unlocked_token += share_type(unlock.amount);
        a.last_unlocked_time = context.current_time();
    });


    xmax_voting::decrease_votes(context, unlock.account, share_type(unlock.amount));
}

#ifdef USE_V8
void handle_xmax_setjscode(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto  msg = context.msg.as<Types::setcode>();

	context.require_authorization(msg.account);

	FC_ASSERT(msg.vm_type == 0);
	FC_ASSERT(msg.vm_version == 0);

	abi_serializer(msg.code_abi).validate();

	const auto& account = db.get<account_object, by_name>(msg.account);
	db.modify(account, [&](auto& a) {
	
		a.code_version = fc::sha256::hash(msg.code.data(), msg.code.size());
		a.code.resize(0);
		a.code.resize(msg.code.size());
		memcpy(a.code.data(), msg.code.data(), msg.code.size());

		a.set_abi(msg.code_abi);
	});

	message_context_xmax init_context(context.mutable_controller, context.mutable_db, context.trx, context.msg, msg.account,0);
	jsvm_xmax::get().init(init_context);

}
#endif

void handle_xmax_setcode(message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto  msg = context.msg.as<Types::setcode>();

	context.require_authorization(msg.account);

	FC_ASSERT(msg.vm_type == 0);
	FC_ASSERT(msg.vm_version == 0);

	/// if an ABI is specified make sure it is well formed and doesn't
	/// reference any undefined types
	abi_serializer(msg.code_abi).validate();


	const auto& account = db.get<account_object, by_name>(msg.account);
	//   wlog( "set code: ${size}", ("size",msg.code.size()));
	db.modify(account, [&](auto& a) {
		/** TODO: consider whether a microsecond level local timestamp is sufficient to detect code version changes*/
		//warning TODO : update setcode message to include the hash, then validate it in validate
		a.code_version = fc::sha256::hash(msg.code.data(), msg.code.size());
		// Added resize(0) here to avoid bug in boost vector container
		a.code.resize(0);
		a.code.resize(msg.code.size());
		memcpy(a.code.data(), msg.code.data(), msg.code.size());

		a.set_abi(msg.code_abi);
	});

	message_context_xmax init_context(context.mutable_controller, context.mutable_db, context.trx, context.msg, msg.account, 0);
	vm_xmax::get().init(init_context);
}


void handle_xmax_votebuilder(message_context_xmax& context)    {
    xmax_voting::vote_builder(context);
}
void handle_xmax_regbuilder(message_context_xmax& context)    {
    xmax_voting::reg_builder(context);
}
void handle_xmax_unregbuilder(message_context_xmax& context)    {
    xmax_voting::unreg_builder(context);
}
void handle_xmax_regproxy(message_context_xmax& context)    {
    xmax_voting::reg_proxy(context);
}
void handle_xmax_unregproxy(message_context_xmax& context)    {
    xmax_voting::unreg_proxy(context);
}


//--------------------------------------------------
void handle_xmax_issueerc2o(Chain::message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto issue_erc20 = context.msg.as<Types::issueerc2o>();

	//Todo: Check creator authorization

	//Todo: Check keys authority validation

	//Check existence
	auto existing_account = db.find<account_object, by_name>(issue_erc20.token_name);
	XMAX_ASSERT(existing_account == nullptr, account_name_exists_exception,
		"Cannot create ERC20 token account named ${name}, as that name is already taken",
		("name", issue_erc20.token_name));

	//Create token account
	create_account_internal(addaccount{ issue_erc20.creator,
		issue_erc20.token_name,
		issue_erc20.owner,
		issue_erc20.active,
		issue_erc20.recovery,
		asset{} }, context.mutable_db, context.current_time());	
	
	
	auto erc20_index = MakeErcTokenIndex(issue_erc20.token_name, issue_erc20.creator);
	bool not_exist = db.find<erc20_token_object, by_token_and_owner>(erc20_index) == nullptr;
	XMAX_ASSERT(not_exist,
		message_validate_exception,
		"Erc20 token:'${t}' already exist with Owner Name: ${o}",
		("t", issue_erc20.owner)("o", issue_erc20.token_name));

	db.create<erc20_token_object>([&issue_erc20](erc20_token_object& tok) {		
		tok.token_name = issue_erc20.token_name;
		tok.owner_name = issue_erc20.creator;
		tok.token_amount = issue_erc20.total_balance.amount;		
		tok.total_supply = issue_erc20.total_balance.amount;
	});


}


//--------------------------------------------------
void handle_xmax_issueerc21(Chain::message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto issue_erc721 = context.msg.as<Types::issueerc21>();

	//Todo: Check creator authorization

	//Todo: Check keys authority validation

	//Check precondition
	auto existing_account = db.find<account_object, by_name>(issue_erc721.token_name);
	XMAX_ASSERT(existing_account == nullptr, account_name_exists_exception,
		"Cannot create ERC721 token account named ${name}, as that name is already taken",
		("name", issue_erc721.token_name));

	auto existing_token_obj = db.find<erc721_token_object, by_token_name>(issue_erc721.token_name);
	XMAX_ASSERT(existing_token_obj == nullptr, message_validate_exception,
		"Erc721 token:'${t}' already exist, the owner is ${owner}",
		("t", issue_erc721.token_name)("owner", existing_token_obj->owner_name));

	
	//Create token account
	create_account_internal(addaccount{ issue_erc721.creator,
										issue_erc721.token_name,
										issue_erc721.owner,
										issue_erc721.active,
										issue_erc721.recovery,
										asset{} }, context.mutable_db, context.current_time());


	//Create erc721 token object
	db.create<erc721_token_object>([&issue_erc721](erc721_token_object& token) {
		token.token_name = issue_erc721.token_name;
		token.owner_name = issue_erc721.creator;		
	});	
}


//--------------------------------------------------
void handle_xmax_minterc21(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto minterc721 = context.msg.as<Types::minterc21>();

	//Todo: Check owner authorization

	//Check precondition
	auto existing_account = db.find<account_object, by_name>(minterc721.token_name);
	XMAX_ASSERT(existing_account != nullptr, message_precondition_exception,
		"Cannot find ERC721 token account named ${name}, as that name does not exist",
		("name", minterc721.token_name));

	auto existing_token_obj = db.find<erc721_token_object, by_token_name>(minterc721.token_name);
	XMAX_ASSERT(existing_token_obj != nullptr, message_validate_exception,
		"Erc721 token:'${t}' does not exist.", ("t", minterc721.token_name));


	const auto& tokenObj = db.get<erc721_token_object, by_token_name>(minterc721.token_name);
	xmax_erc721_id token_id{ minterc721.token_id };
	XMAX_ASSERT(tokenObj.minted_tokens.find(token_id) == tokenObj.minted_tokens.end(),
		message_validate_exception,
		"The ERC721 token: ${token_name} with token id:${token_id} has already minted",
		("token_name", minterc721.token_name)("token_id", minterc721.token_id));


	db.modify(tokenObj, [&minterc721](erc721_token_object& obj) {
		obj.minted_tokens.insert(xmax_erc721_id(minterc721.token_id));
	});

}

//--------------------------------------------------
void handle_xmax_revoketoken(Chain::message_context_xmax& context)
{

}

	} // namespace Native
} // namespace Xmaxplatform

