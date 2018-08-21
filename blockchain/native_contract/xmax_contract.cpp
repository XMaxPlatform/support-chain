/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <xmax_contract.hpp>
#include <blockchain_exceptions.hpp>
#include <chain_xmax.hpp>
#include <message_context_xmax.hpp>
#include <message_xmax.hpp>
#include <authoritys_utils.hpp>

#include <objects/object_utility.hpp>
#include <objects/account_object.hpp>
#include <objects/contract_object.hpp>
#include <objects/authority_object.hpp>
#include <objects/linked_permission_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/resource_token_object.hpp>
#include <objects/chain_object_table.hpp>
#include <objects/static_config_object.hpp>
#include <objects/erc20_token_object.hpp>
#include <objects/erc20_token_account_object.hpp>
#include <objects/erc721_token_object.hpp>
#include <objects/erc721_token_account_object.hpp>
#include <xmax_voting.hpp>
#include <vm_xmax.hpp>
#include <safemath.hpp>

#include <abi_serializer.hpp>

#ifdef USE_V8
#include <jsvm_xmax.hpp>
#endif



namespace Xmaxplatform {
    namespace Native_contract {
using namespace Chain;
namespace Config = ::Xmaxplatform::Config;
namespace Chain = ::Xmaxplatform::Chain;
namespace SafeMath = Xmaxplatform::Basetypes::SafeMath;
using namespace ::Xmaxplatform::Basetypes;

typedef mutable_db_table <xmx_token_object, by_owner_name> xmax_token_table;
typedef mutable_db_table <resource_token_object, by_owner_name> resource_token_table;


static void validate_authority(const message_context_xmax& context, const Basetypes::authority& auth) {

	for (const account_permission_weight& a : auth.accounts)
	{
		const account_object* acct = context.db.find<account_object, by_name>(a.permission.account);
		XMAX_ASSERT( acct != nullptr, message_validate_exception, "account '${account}' does not exist", ("account", a.permission.account) );

		if (a.permission.authority == Config::xmax_owner_name || a.permission.authority == Config::xmax_active_name)
			continue; // all account has owner and active permissions.

		try {
			utils::get_authority_object(context.db, a.permission);
		}
		catch (...) 
		{
			XMAX_THROW(message_validate_exception,
				"permission '${perm}' does not exist",
				("perm", a.permission)
			);
		}
	}
}

static const account_object& account_assert(const Basechain::database& db, const account_name accout)
{
	const account_object* acc = db.find<account_object, by_name>(accout);
	XMAX_ASSERT(acc != nullptr, message_validate_exception,
		"Account not found: ${account}", ("account", accout));
	return *acc;
}

template <class ErcObjectType>
static void validate_token_not_revoke(const ErcObjectType& erc_object) {
	XMAX_ASSERT(erc_object.revoked == 0, message_validate_exception,
		"ERC token:${token_name} already revoked!", ("token_name", erc_object.token_name));
}

template <class ErcObjectType>
static void validate_token_canmint(const ErcObjectType& erc_object) {
	XMAX_ASSERT(erc_object.stopmint == 0, message_validate_exception,
		"ERC token:${token_name} already revoked!", ("token_name", erc_object.token_name));
}

static void validate_name(const name& n) {
	XMAX_ASSERT(n.valid(), message_validate_exception,
		"Name:${name} is invalid", ("name", n));
}

static const account_object& xmax_new_account(Basechain::database& db, account_name creator, time creation_time,  account_type acc_type)
{
	const auto& new_account = db.create<account_object>([&](account_object& a) {
		a.name = creator;
		a.type = acc_type;
		a.creation_date = creation_time;
	});

	return new_account;
}

//API handler implementations
void handle_xmax_addaccount(message_context_xmax& context) {
	Types::addaccount create = context.msg.as<Types::addaccount>();
	Basechain::database& db = context.mutable_db;
	time current_time = context.current_time();

	context.require_authorization(create.creator);

	auto existing_account = db.find<account_object, by_name>(create.name);
	XMAX_ASSERT(existing_account == nullptr, account_name_exists_exception,
		"Cannot create account named ${name}, as that name is already taken",
		("name", create.name.to_string()));

	XMAX_ASSERT(create.deposit.amount > 0, message_validate_exception,
		"account creation deposit must > 0, deposit: ${a}", ("a", create.deposit));

	const auto& new_account = xmax_new_account(db, create.name, current_time, Chain::acc_personal);


	const auto& creatorToken = db.get<xmx_token_object, by_owner_name>(create.creator);

	XMAX_ASSERT(creatorToken.main_token >= create.deposit.amount, message_validate_exception,
		"Creator '${c}' has insufficient funds to make account creation deposit of ${a}",
		("c", create.creator.to_string())("a", create.deposit));

	db.modify(creatorToken, [&create](xmx_token_object& b) {
		b.main_token -= create.deposit.amount;
	});

	db.create<xmx_token_object>([&create](xmx_token_object& b) {
		b.owner_name = create.name;
		b.main_token = create.deposit.amount;
	});

	auto owner_p = db.create<authority_object>([&](authority_object& obj)
	{
		obj.auth_name = Config::xmax_owner_name;

		obj.parent = 0;
		obj.owner_name = create.name;
		obj.authoritys = create.owner;
		obj.last_updated = current_time;
	});

	auto owner_a = db.create<authority_object>([&](authority_object& obj)
	{
		obj.auth_name = Config::xmax_active_name;

		obj.parent = owner_p.id;
		obj.owner_name = create.name;
		obj.authoritys = create.owner;
		obj.last_updated = current_time;
	});

}


void handle_xmax_addcontract(Chain::message_context_xmax& context)
{
	const Types::addcontract& msgdata = context.msg.as<Types::addcontract>();
	Basechain::database& db = context.mutable_db;
	time current_time = context.current_time();

	account_name contract_name = msgdata.name;

	context.require_authorization(msgdata.creator);

	const auto& new_account = xmax_new_account(db, contract_name, current_time, Chain::acc_contract);

	abi_serializer(msgdata.code_abi).validate();

	const auto& contract = db.get<contract_object, by_name>(contract_name);
	db.modify(contract, [&](contract_object& a) {

		a.code_version = fc::sha256::hash(msgdata.code.data(), msgdata.code.size());
		a.code.resize(0);
		a.code.resize(msgdata.code.size());
		memcpy(a.code.data(), msgdata.code.data(), msgdata.code.size());

		a.set_abi(msgdata.code_abi);
	});

	message_context_xmax init_context(context.mutable_chain, context.mutable_db, context.trx, context.msg, contract_name, 0);
	jsvm_xmax::get().init(init_context);

}

void handle_xmax_adderc20(Chain::message_context_xmax& context)
{
	const Types::adderc20& msgdata = context.msg.as<Types::adderc20>();
	Basechain::database& db = context.mutable_db;
	time current_time = context.current_time();

	account_name contract_name = msgdata.name;

	context.require_authorization(msgdata.creator);

	const auto& new_account = xmax_new_account(db, contract_name, current_time, Chain::acc_erc20);
}
void handle_xmax_adderc721(Chain::message_context_xmax& context)
{
	const Types::adderc20& msgdata = context.msg.as<Types::adderc20>();
	Basechain::database& db = context.mutable_db;
	time current_time = context.current_time();

	account_name contract_name = msgdata.name;

	context.require_authorization(msgdata.creator);

	const auto& new_account = xmax_new_account(db, contract_name, current_time, Chain::acc_erc721);
}

void handle_xmax_updateauth(Chain::message_context_xmax& context)
{
	Types::updateauth msg = context.msg.as<Types::updateauth>();

	context.require_authorization(msg.account);

	auto& data_db = context.db;

	XMAX_ASSERT(!msg.permission.empty(), message_validate_exception, "Cannot create authority with empty name");

	XMAX_ASSERT(utils::check_authority_name(msg.permission), message_validate_exception, "Permission names cannot be started with 'xmax'");

	XMAX_ASSERT(msg.permission != msg.parent, message_validate_exception, "Cannot set an authority as its own parent");


	account_assert(data_db, msg.account); // check account.

	XMAX_ASSERT(utils::validate_weight_format(msg.new_authority), message_validate_exception, "Invalid authority: ${auth}", ("auth", msg.new_authority));

	if (msg.permission == Config::xmax_owner_name)
		XMAX_ASSERT(msg.parent.empty(), message_validate_exception, "Owner authority's parent must be empty");
	else
		XMAX_ASSERT(!msg.parent.empty(), message_validate_exception, "Only owner authority's parent can be empty");

	if (msg.permission == Config::xmax_active_name)
		XMAX_ASSERT(msg.parent == Config::xmax_owner_name, message_validate_exception, "Active authority's parent must be owner", ("msg.parent", msg.parent));


	validate_authority(context, msg.new_authority);

	const authority_object* old_auth = utils::find_authority_object(context.db, {msg.account, msg.permission});

	authority_object::id_type parent_id = 0;
	if (msg.permission != Config::xmax_owner_name)
	{
		auto auth = utils::get_authority_object(context.db, { msg.account, msg.parent });
		parent_id = auth.id;
	}

	if (old_auth) // update auth.
	{
		XMAX_ASSERT(parent_id == old_auth->parent, message_validate_exception, "Changing parent authority is not supported");

		utils::modify_authority_object(context.mutable_db, *old_auth, msg.new_authority, context.chain.building_block_timestamp().time_point());

	}
	else // new auth.
	{
		utils::new_authority_object(context.mutable_db, msg.account, msg.permission, parent_id, msg.new_authority, context.chain.building_block_timestamp().time_point());
	}
}

void handle_xmax_deleteauth(Chain::message_context_xmax& context)
{
	Types::deleteauth msg = context.msg.as<Types::deleteauth>();

	context.require_authorization(msg.account);

	XMAX_ASSERT(msg.permission != Config::xmax_active_name, message_validate_exception, "Cannot delete active authority");
	XMAX_ASSERT(msg.permission != Config::xmax_owner_name, message_validate_exception, "Cannot delete owner authority");

	utils::remove_linked_object(context.mutable_db, msg.account, msg.permission);

	const auto& auth = utils::get_authority_object(context.db, { msg.account, msg.permission });

	utils::remove_authority_object(context.mutable_db, auth);

}

void handle_xmax_linkauth(Chain::message_context_xmax& context)
{
	Types::linkauth msg = context.msg.as<Types::linkauth>();
	try {
		XMAX_ASSERT(!msg.requirement.empty(), message_validate_exception, "Required authority name cannot be empty");

		context.require_authorization(msg.account);

		account_assert(context.mutable_db, msg.account);
		account_assert(context.mutable_db, msg.code);

		{
			const authority_object* auth = context.db.find<authority_object, by_owner>(msg.requirement);
			XMAX_ASSERT(auth != nullptr, message_validate_exception, "Auth not found: ${auth}", ("auth", msg.requirement));
		}

		auto tlkey = std::make_tuple(msg.account, msg.code, msg.type);

		const auto linked = context.db.find<linked_permission_object, by_func>(tlkey);

		if (linked)
		{
			XMAX_ASSERT(linked->required_auth != msg.requirement, message_validate_exception, "Auth used: ${auth}", ("auth", msg.requirement));
			context.mutable_db.modify(*linked, [auth = msg.requirement](linked_permission_object& linked) {
				linked.required_auth = auth;
			});
		}
		else
		{
			context.mutable_db.create<linked_permission_object>([&msg](linked_permission_object& link) {
				link.account = msg.account;
				link.code = msg.code;
				link.func = msg.type;
				link.required_auth = msg.requirement;
			});
		}
		  

	} FC_CAPTURE_AND_RETHROW((msg))
}

void handle_xmax_unlinkauth(Chain::message_context_xmax& context)
{
	Types::unlinkauth msg = context.msg.as<Types::unlinkauth>();

	context.require_authorization(msg.account);

	auto tlkey = std::make_tuple(msg.account, msg.code, msg.type);

	const auto linked = context.db.find<linked_permission_object, by_func>(tlkey);

	XMAX_ASSERT(linked != nullptr, message_validate_exception, "Linked permission not found");
	context.mutable_db.remove(*linked);
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

      XMAX_ASSERT(from.main_token >= transfer.amount, message_precondition_exception, "Insufficient Funds",
                 ("from.main_token",from.main_token)("transfer.amount",transfer.amount));

      const auto& to = db.get<xmx_token_object, by_owner_name>(transfer.to);
      db.modify(from, [&](xmx_token_object& a) {
         a.main_token -= share_type(transfer.amount);
      });
      db.modify(to, [&](xmx_token_object& a) {
         a.main_token += share_type(transfer.amount);
      });
   } FC_CAPTURE_AND_RETHROW( (transfer) ) 
}

void handle_xmax_lock(message_context_xmax& context) {
    auto lock = context.msg.as<Types::lock>();

    XMAX_ASSERT(lock.amount > 0, message_validate_exception, "Locked amount must be positive");

    context.require_scope(lock.to);
    context.require_scope(lock.from);
    context.require_scope(Config::xmax_contract_name);

    context.require_authorization(lock.from);

    context.require_recipient(lock.to);
    context.require_recipient(lock.from);

    xmax_token_table xmax_token_tbl(context.mutable_db);
    resource_token_table resource_token_tbl(context.mutable_db);

    const xmx_token_object& locker = xmax_token_tbl.get(lock.from);

    XMAX_ASSERT( locker.main_token >= lock.amount, message_precondition_exception,
                 "Account ${a} lacks sufficient funds to lock ${amt} SUP", ("a", lock.from)("amt", lock.amount)("available",locker.main_token) );

    const auto& resource_token = resource_token_tbl.get(lock.to);

    xmax_token_tbl.modify(locker, [&lock](xmx_token_object& a) {
        a.main_token -= share_type(lock.amount);
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

	const auto& contract = db.get<contract_object, by_name>(msg.account);
	db.modify(contract, [&](auto& a) {
	
		a.code_version = fc::sha256::hash(msg.code.data(), msg.code.size());
		a.code.resize(0);
		a.code.resize(msg.code.size());
		memcpy(a.code.data(), msg.code.data(), msg.code.size());

		a.set_abi(msg.code_abi);
	});

	message_context_xmax init_context(context.mutable_chain, context.mutable_db, context.trx, context.msg, msg.account,0);
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


	const auto& contract = db.get<contract_object, by_name>(msg.account);
	//   wlog( "set code: ${size}", ("size",msg.code.size()));
	db.modify(contract, [&](auto& a) {
		/** TODO: consider whether a microsecond level local timestamp is sufficient to detect code version changes*/
		//warning TODO : update setcode message to include the hash, then validate it in validate
		a.code_version = fc::sha256::hash(msg.code.data(), msg.code.size());
		// Added resize(0) here to avoid bug in boost vector container
		a.code.resize(0);
		a.code.resize(msg.code.size());
		memcpy(a.code.data(), msg.code.data(), msg.code.size());

		a.set_abi(msg.code_abi);
	});

	message_context_xmax init_context(context.mutable_chain, context.mutable_db, context.trx, context.msg, 0);
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
void handle_xmax_issueerc20(Chain::message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto issue_erc20 = context.msg.as<Types::issueerc20>();

	//Todo: Check creator authorization
	context.require_authorization(issue_erc20.creator);
	//Todo: Check keys authority validation

	//Check existence
	auto existing_token_obj = db.find<erc20_token_object, by_token_name>(issue_erc20.token_name);
	XMAX_ASSERT(existing_token_obj == nullptr, message_validate_exception,
		"Erc20 token:'${t}' already exist, the owner is ${owner}",
		("t", issue_erc20.token_name)("owner", existing_token_obj->owner_name));


	db.create<erc20_token_object>([&issue_erc20](erc20_token_object& obj) {		
		obj.token_name = issue_erc20.token_name;
		obj.owner_name = issue_erc20.creator;
		obj.balance = issue_erc20.total_balance.amount;
		obj.total_supply = issue_erc20.total_balance.amount;
	});


}


//--------------------------------------------------
void handle_xmax_issueerc721(Chain::message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto issue_erc721 = context.msg.as<Types::issueerc721>();

	//Todo: Check creator authorization

	//Todo: validate the owner active recovery authority
	

	//Check precondition
	auto existing_token_obj = db.find<erc721_token_object, by_token_name>(issue_erc721.token_name);
	XMAX_ASSERT(existing_token_obj == nullptr, message_validate_exception,
		"Erc721 token:'${t}' already exist, the owner is ${owner}",
		("t", issue_erc721.token_name)("owner", existing_token_obj->owner_name));

	
	//Create erc721 token object
	db.create<erc721_token_object>([&issue_erc721](erc721_token_object& token) {
		token.token_name = issue_erc721.token_name;
		token.owner_name = issue_erc721.creator;		
	});	
}


//--------------------------------------------------
void handle_xmax_minterc20(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto minterc20 = context.msg.as<Types::minterc20>();

	//Todo: Check owner authorization

	//Check precondition
	auto& existing_token_obj = db.get<erc20_token_object, by_token_name>(minterc20.token_name);
	validate_token_not_revoke(existing_token_obj);

	validate_token_canmint(existing_token_obj);
	//Check owner authorization
	context.require_authorization(existing_token_obj.owner_name);//must signed by token sender.

	db.modify(existing_token_obj, [&minterc20](erc20_token_object& obj) {
		obj.total_supply += minterc20.mint_amount.amount;
		obj.balance += minterc20.mint_amount.amount;
	});
}
//--------------------------------------------------
void handle_xmax_stopminterc20(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto stopminterc20 = context.msg.as<Types::stopminterc20>();

	//Check precondition
	auto& existing_token_obj = db.get<erc20_token_object, by_token_name>(stopminterc20.token_name);

	validate_token_not_revoke(existing_token_obj);

	//Check owner authorization
	context.require_authorization(existing_token_obj.owner_name);//must signed by token sender.
	validate_token_canmint(existing_token_obj);

	db.modify(existing_token_obj, [](erc20_token_object& obj) {
		obj.stopmint = 1;
	});
}
//--------------------------------------------------
void handle_xmax_minterc721(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto minterc721 = context.msg.as<Types::minterc721>();

	//Todo: Check owner authorization

	//Check precondition

	const auto& existing_token_obj = db.get<erc721_token_object, by_token_name>(minterc721.token_name);
	validate_token_not_revoke(existing_token_obj);
	xmax_erc721_id token_id{ minterc721.token_id };
	XMAX_ASSERT(existing_token_obj.minted_tokens.find(token_id) == existing_token_obj.minted_tokens.end(),
		message_validate_exception,
		"The ERC721 token: ${token_name} with token id:${token_id} has already minted",
		("token_name", minterc721.token_name)("token_id", minterc721.token_id));


	db.modify(existing_token_obj, [&minterc721](erc721_token_object& obj) {
		obj.minted_tokens.insert(xmax_erc721_id(minterc721.token_id));
	});

}

//--------------------------------------------------
void handle_xmax_revokeerc20(Chain::message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto revokeerc2o = context.msg.as<Types::revokeerc20>();

	//Todo: Check owner authorization

	//Check precondition
	auto& erc20_obj = db.get<erc20_token_object, by_token_name>(revokeerc2o.token_name);	
	validate_token_not_revoke(erc20_obj);
	db.modify(erc20_obj, [](erc20_token_object& obj) {
		obj.revoked = 1;
	});

}


//--------------------------------------------------
void handle_xmax_transfererc20(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto transfer_erc20 = context.msg.as<Types::transfererc20>();

	//Todo: Check owner authorization

	validate_name(transfer_erc20.to);

	auto& erc20_obj = db.get<erc20_token_object, by_token_name>(transfer_erc20.token_name);
	validate_token_not_revoke(erc20_obj);

	//Validate transfer amount
	XMAX_ASSERT(erc20_obj.balance >= transfer_erc20.value.amount,
		message_validate_exception,
		"The ERC20 token: ${token_name} transfer amount: ${amount} invalid.",
		("token_name", transfer_erc20.token_name)("amount", transfer_erc20.value));

	//Create token account if no exist

	auto erc20_index = MakeErcTokenIndex(transfer_erc20.token_name, transfer_erc20.to);
	auto token_account_ptr = db.find<erc20_token_account_object, by_token_and_owner>(erc20_index);
	if (!token_account_ptr)
	{		
		db.create<erc20_token_account_object>([&transfer_erc20](erc20_token_account_object &obj) {
			obj.balance = 0;
			obj.owner_name = transfer_erc20.to;
			obj.token_name = transfer_erc20.token_name;			
		});
	}

	const auto& token_account = db.get<erc20_token_account_object, by_token_and_owner>(erc20_index);
	db.modify(erc20_obj, [&transfer_erc20](erc20_token_object& obj) {
		obj.balance = SafeMath::sub(obj.balance, transfer_erc20.value.amount);				
	});

	db.modify(token_account, [&transfer_erc20](erc20_token_account_object& account) {		
		account.balance = SafeMath::add(account.balance, transfer_erc20.value.amount);
	});	
	
	//TODO: emit Transfer event
}


//--------------------------------------------------
void handle_xmax_transferfromerc20(Chain::message_context_xmax& context)
{
	auto& db = context.mutable_db;
	auto transfer_erc20 = context.msg.as<Types::transferfromerc20>();

	//Check sender authorization
	context.require_authorization(transfer_erc20.from);//must signed by token sender.
	
	validate_name(transfer_erc20.from);
	validate_name(transfer_erc20.to);
	
	auto& erc20_obj = db.get<erc20_token_object, by_token_name>(transfer_erc20.token_name);
	validate_token_not_revoke(erc20_obj);

	auto& from_token_account = db.get<erc20_token_account_object, by_token_and_owner>(MakeErcTokenIndex(transfer_erc20.token_name, transfer_erc20.from));
	auto& to_token_account = db.get<erc20_token_account_object, by_token_and_owner>(MakeErcTokenIndex(transfer_erc20.token_name, transfer_erc20.to));

	//Validate transfer amount
	XMAX_ASSERT(transfer_erc20.value.amount <= from_token_account.balance,
		message_validate_exception,
		"Not enough balance for transfering ${amount} ERC20 token:${token_name} at account:${account}",
		("token_name", transfer_erc20.token_name)("amount", transfer_erc20.value.amount)("account", transfer_erc20.from));

	db.modify(from_token_account, [&transfer_erc20](erc20_token_account_object& from_acc){		
		from_acc.balance = SafeMath::sub(from_acc.balance, transfer_erc20.value.amount);
	});

	db.modify(to_token_account, [&transfer_erc20](erc20_token_account_object& to_acc) {
		to_acc.balance = SafeMath::add(to_acc.balance, transfer_erc20.value.amount);		
	});

	//TODO: emit transfer event
}

//--------------------------------------------------
void handle_xmax_revokeerc721(Chain::message_context_xmax& context) {
	auto& db = context.mutable_db;
	auto revokeerc721 = context.msg.as<Types::revokeerc721>();

	//Todo: Check owner authorization
	
	//Check precondition
	auto& erc721_obj = db.get<erc721_token_object, by_token_name>(revokeerc721.token_name);
	validate_token_not_revoke(erc721_obj);
	db.modify(erc721_obj, [](erc721_token_object& obj) {
		obj.revoked = 1;
	});
}

	} // namespace Native
} // namespace Xmaxplatform

