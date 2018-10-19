/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */

#include <blockchain_exceptions.hpp>
#include <mongodb_plugin.hpp>


#ifdef MONGO_DB

#include <abi_serializer.hpp>
#include <native_contract_chain_init.hpp>

#include <queue>

#include <fc/io/json.hpp>
#include <fc/variant.hpp>

#include <boost/atomic/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>




const static char* DEFAULT_MONGODB_NAME = "XMAX";

const static std::string col_accounts_name = "Accounts";
const static std::string col_blocks_name = "Blocks";
const static std::string col_transactions_name = "Transactions";
const static std::string col_messages_name = "Messages";

const static uint32_t DEFAULT_QUEUE_LENGTH = 256;
const static float WARNING_QUEUE_LENGTH_PERCENT = 0.7f;


#endif


namespace fc { class variant; }

namespace Xmaxplatform {

	/*!
	 * \class mongodb_plugin_impl
	 *
	 */
#ifndef MONGO_DB
	class mongodb_plugin_impl
	{
	public:
		mongodb_plugin_impl() {}
	};
#else

	using Xmaxplatform::Basetypes::account_name;
	using Xmaxplatform::Basetypes::func_name;
	using Xmaxplatform::Basetypes::authority_name;

	using Xmaxplatform::Chain::xmax_type_block_id;
	using Xmaxplatform::Chain::processed_transaction;
	using Xmaxplatform::Chain::signed_block;
	using Xmaxplatform::Chain::xmax_type_transaction_id;



  

	class mongodb_plugin_impl
	{
	public:
		mongodb_plugin_impl();
		~mongodb_plugin_impl();

		void finalize_block(const signed_block& block);
		void handle_finalized_block(const signed_block& block);
		
		void initialize();
		void drop_tables();
		void handle_blocks();

		void update_account(const Chain::message_xmax& msg);

		

	private:
		void _handle_finalized_block(const signed_block& block);
		void _check_block_state(const signed_block& block);
		
		
		//Insert methods
		void _insert_block(const signed_block& block);
		inline void _insert_block_transactions(const signed_block& block, bsoncxx::builder::stream::document& block_doc);
		template <class T>
		bool _insert_single_transaction(T& trans_arr_ctx, const signed_block& block, uint32_t trans_idx, 
			const Chain::signed_transaction& trans, mongocxx::bulk_write& bulk_trans);
		template <class TRANS_CTX_TYPE>
		inline void _insert_single_message(int32_t idx, TRANS_CTX_TYPE& trans_ctx, const signed_block& block, 
			mongocxx::bulk_write& bulk_msgs, const Xmaxplatform::Basetypes::message& message, const Chain::signed_transaction& trans);

		//Update methods
		void _update_msg_transfer(const Chain::message_xmax& msg);
		void _update_msg_addaccount(const Chain::message_xmax& msg);
		void _update_msg_lock(const Chain::message_xmax& msg);
		void _update_msg_unlock(const Chain::message_xmax& msg);
		void _update_msg_claim(const Chain::message_xmax& msg);
		void _update_msg_setcode(const Chain::message_xmax& msg);;

		//Utility methods
		inline bool _account_filter_include(const Xmaxplatform::Basetypes::vector<account_name>& scope) {
			for (auto& acc : scope) {
				if (account_filter.count(acc))
					return true;
			}

			return false;
		}

		//DB table relative interfaces
		void _init_mongodb_tables();

	public:
		static const func_name addaccount;
		static const func_name transfer;
		static const func_name lock;
		static const func_name unlock;
		static const func_name claim;
		static const func_name setcode;


		bool initialized{ false };
		std::string mongodb_name;
		mongocxx::client mongo_client;
		mongocxx::instance mongo_instance;

		bool drop_tables_on_init{ false };
		boost::atomic<bool> started{ false };

		static Basetypes::abi xmax_abi;
		mongocxx::collection accounts;

		size_t handled_block_num{ 0 };
		boost::thread working_thread;
		std::queue<signed_block> block_queue;
		uint32_t block_queue_length;
		boost::mutex block_queue_mutex;	
		boost::condition_variable condtion;
		boost::atomic<bool> work_done{ false };
		std::set<account_name> account_filter;
	};

	Basetypes::abi mongodb_plugin_impl::xmax_abi;

	const func_name mongodb_plugin_impl::addaccount = "addaccount";
	const func_name mongodb_plugin_impl::transfer = "transfer";
	const func_name mongodb_plugin_impl::lock = "lock";
	const func_name mongodb_plugin_impl::unlock = "unlock";
	const func_name mongodb_plugin_impl::claim = "claim";
	const func_name mongodb_plugin_impl::setcode = "setcode";


	void mongodb_plugin_impl::handle_blocks() {
		try {
			signed_block block;
			size_t size = 0;
			while (true) {
				boost::mutex::scoped_lock lock(block_queue_mutex);
				while (block_queue.empty() && !work_done) {
					condtion.wait(lock);
				}
				size = block_queue.size();
				if (size > 0) {
					block = block_queue.front();
					block_queue.pop();
					lock.unlock();
					// warn if queue size greater than WARNING_QUEUE_LENGTH_PERCENT
					if (size > (block_queue_length * WARNING_QUEUE_LENGTH_PERCENT)) {
						wlog("queue size: ${q}", ("q", size + 1));
					}
					else if (work_done) {
						ilog("draining queue, size: ${q}", ("q", size + 1));
					}
					handle_finalized_block(block);
					continue;
				}
				else if (work_done) {
					break;
				}
			}
			ilog("mongodb_plugin consum thread shutdown gracefully");
		}
		catch (fc::exception& e) {
			elog("FC Exception while consuming block ${e}", ("e", e.to_string()));
		}
		catch (std::exception& e) {
			elog("STD Exception while consuming block ${e}", ("e", e.what()));
		}
		catch (...) {
			elog("Unknown exception while consuming block");
		}
	}


	

	//--------------------------------------------------
	mongodb_plugin_impl::mongodb_plugin_impl()
		: mongo_instance{},
		mongo_client{}
	{

	}

	//--------------------------------------------------
	mongodb_plugin_impl::~mongodb_plugin_impl()
	{
		try {
			work_done = true;
			condtion.notify_one();
			//consum_thread.join();
		}
		catch (std::exception& e) {
			elog("Exception on mongodb_plugin shutdown of consum thread: ${e}", ("e", e.what()));
		}
	}


	//--------------------------------------------------
	void mongodb_plugin_impl::finalize_block(const signed_block& block)
	{
		try {
			if (started) {
				boost::mutex::scoped_lock lock(block_queue_mutex);
				block_queue.push(block);
				lock.unlock();
				condtion.notify_one();
			}
			else {
				handle_finalized_block(block);
			}

		} 
		catch (fc::exception& e) 
		{
			elog("fc::exception occured in mongodb_plugin_impl::finalize_block ${e}", ("e", e.to_string()));
		} 
		catch (std::exception& e) 
		{
			elog("std::exception occured in mongodb_plugin_impl::finalize_block ${e}", ("e", e.what()));
		}
		catch (...) {
			elog("Unknown exception occured in mongodb_plugin_impl::finalize_block.");
		}

	}

	//--------------------------------------------------
	void mongodb_plugin_impl::handle_finalized_block(const signed_block& block)
	{
		try {			
			_handle_finalized_block(block);
		}
		catch (fc::exception& e)
		{
			elog("fc::exception occured in mongodb_plugin_impl::finalize_block ${e}", ("e", e.to_string()));
		}
		catch (std::exception& e)
		{
			elog("std::exception occured in mongodb_plugin_impl::finalize_block ${e}", ("e", e.what()));
		}
		catch (...) {
			elog("Unknown exception occured in mongodb_plugin_impl::finalize_block.");
		}
	}



	namespace {

		auto find_account(mongocxx::collection& accounts, const account_name& name) {
			using bsoncxx::builder::stream::document;
			document find_acc{};
			find_acc << "name" << name.to_string();
			auto account = accounts.find_one(find_acc.view());
			if (!account) {
				FC_THROW("Unable to find account ${n}", ("n", name));
			}
			return *account;
		}

		auto find_transaction(mongocxx::collection& trans, const std::string& trx_id_str) {
			using bsoncxx::builder::basic::make_document;
			using bsoncxx::builder::basic::kvp;
			return trans.find_one(make_document(kvp("transaction_id", trx_id_str)));
		}


		void add_data(bsoncxx::builder::basic::document& msg_doc,
			mongocxx::collection& accounts,
			const Chain::message_xmax& msg)
		{
			using bsoncxx::builder::basic::kvp;
			try {
				Basetypes::abi_serializer abis;
				if (msg.code == Config::xmax_contract_name) {
					abis.set_abi(mongodb_plugin_impl::xmax_abi);
				}
				else {
					auto from_account = find_account(accounts, msg.code);
					auto abi = fc::json::from_string(bsoncxx::to_json(from_account.view()["abi"].get_document())).as<Basetypes::abi>();
					abis.set_abi(abi);
				}
				auto v = abis.binary_to_variant(abis.get_action_type(msg.type), msg.data);
				auto json = fc::json::to_string(v);
				try {
					const auto& value = bsoncxx::from_json(json);
					msg_doc.append(kvp("data", value));
					return;
				}
				catch (std::exception& e) {
					elog("Unable to convert XMAX JSON to MongoDB JSON: ${e}", ("e", e.what()));
					elog("  XMAX JSON: ${j}", ("j", json));
				}
			}
			catch (fc::exception& e) {
				elog("Unable to convert message.data to ABI type: ${t}, what: ${e}", ("t", msg.type)("e", e.to_string()));
			}
			catch (std::exception& e) {
				elog("Unable to convert message.data to ABI type: ${t}, std what: ${e}", ("t", msg.type)("e", e.what()));
			}
			catch (...) {
				elog("Unable to convert message.data to ABI type: ${t}", ("t", msg.type));
			}
			// if anything went wrong just store raw hex_data
			msg_doc.append(kvp("hex_data", fc::variant(msg.data).as_string()));
		}
	}

	//--------------------------------------------------
	void mongodb_plugin_impl::update_account(const Chain::message_xmax& msg) {
		
		if (msg.code != Config::xmax_contract_name)
			return;

		if (msg.type == transfer) {
			_update_msg_transfer(msg);
		}
		else if (msg.type == addaccount)
		{
			_update_msg_addaccount(msg);
		}
		else if (msg.type == lock)
		{
			_update_msg_lock(msg);
		}
		else if (msg.type == unlock)
		{
			_update_msg_unlock(msg);
		}
		else if (msg.type == claim)
		{
			_update_msg_claim(msg);
		}
		else if (msg.type == setcode)
		{
			_update_msg_setcode(msg);
		}

	}

	//--------------------------------------------------
	void mongodb_plugin_impl::_update_msg_transfer(const Chain::message_xmax& msg)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::stream;

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		auto transfer = msg.as<Basetypes::transfer>();
		auto from_name = transfer.from.to_string();
		auto to_name = transfer.to.to_string();
		auto from_account = find_account(accounts, transfer.from);
		auto to_account = find_account(accounts, transfer.to);

		asset from_balance = asset::from_string(from_account.view()["main_token"].get_utf8().value.to_string());
		asset to_balance = asset::from_string(to_account.view()["main_token"].get_utf8().value.to_string());
		from_balance -= Xmaxplatform::Basetypes::share_type(transfer.amount);
		to_balance += Xmaxplatform::Basetypes::share_type(transfer.amount);

		document update_from{};
		update_from << "$set" << open_document << "main_token" << from_balance.to_string()
			<< "updatedTime" << b_date{ now }
		<< close_document;
		document update_to{};
		update_to << "$set" << open_document << "main_token" << to_balance.to_string()
			<< "updatedTime" << b_date{ now }
		<< close_document;

		accounts.update_one(document{} << "_id" << from_account.view()["_id"].get_oid() << finalize, update_from.view());
		accounts.update_one(document{} << "_id" << to_account.view()["_id"].get_oid() << finalize, update_to.view());
	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_update_msg_addaccount(const Chain::message_xmax& msg)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::stream;

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		auto addaccount = msg.as<Basetypes::addaccount>();

		// find creator to update its balance
		auto from_name = addaccount.creator.to_string();
		document find_from{};
		find_from << "name" << from_name;
		auto from_account = accounts.find_one(find_from.view());
		if (!from_account) {
			elog("Unable to find account ${n}", ("n", from_name));
			return;
		}
		// decrease creator by deposit amount
		auto from_view = from_account->view();
		asset from_balance = asset::from_string(from_view["main_token"].get_utf8().value.to_string());
		from_balance -= addaccount.deposit;
		document update_from{};
		update_from << "$set" << open_document << "main_token" << from_balance.to_string() << close_document;
		accounts.update_one(find_from.view(), update_from.view());

		// create new account with staked deposit amount
		bsoncxx::builder::stream::document doc{};
		doc << "name" << addaccount.name.to_string()
			<< "main_token" << asset().to_string()
			<< "staked_token" << addaccount.deposit.to_string()
			<< "unstaking_token" << asset().to_string()
			<< "createdTime" << b_date{ now }
		<< "updatedTime" << b_date{ now };
		if (!accounts.insert_one(doc.view())) {
			elog("Failed to insert account ${n}", ("n", addaccount.name));
		}
	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_update_msg_lock(const Chain::message_xmax& msg)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::stream;

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		auto lock = msg.as<Basetypes::lock>();
		auto from_account = find_account(accounts, lock.from);
		auto to_account = find_account(accounts, lock.to);

		asset from_balance = asset::from_string(from_account.view()["main_token"].get_utf8().value.to_string());
		asset to_balance = asset::from_string(to_account.view()["staked_token"].get_utf8().value.to_string());
		from_balance -= lock.amount;
		to_balance += lock.amount;

		document update_from{};
		update_from << "$set" << open_document << "main_token" << from_balance.to_string()
			<< "updatedTime" << b_date{ now }
		<< close_document;
		document update_to{};
		update_to << "$set" << open_document << "staked_token" << to_balance.to_string()
			<< "updatedTime" << b_date{ now }
		<< close_document;

		accounts.update_one(document{} << "_id" << from_account.view()["_id"].get_oid() << finalize, update_from.view());
		accounts.update_one(document{} << "_id" << to_account.view()["_id"].get_oid() << finalize, update_to.view());
	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_update_msg_unlock(const Chain::message_xmax& msg)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::stream;

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		auto unlock = msg.as<Basetypes::unlock>();
		auto from_account = find_account(accounts, unlock.account);

		asset unstack_token = asset::from_string(from_account.view()["unstacking_token"].get_utf8().value.to_string());
		asset stack_token = asset::from_string(from_account.view()["stacked_token"].get_utf8().value.to_string());
		auto deltaStake = unstack_token - unlock.amount;
		stack_token += deltaStake;
		unstack_token = unlock.amount;
		// TODO: proxies and last_unstaking_time

		document update_from{};
		update_from << "$set" << open_document
			<< "stacked_token" << stack_token.to_string()
			<< "unstacking_token" << unstack_token.to_string()
			<< "updatedTime" << b_date{ now }
		<< close_document;

		accounts.update_one(document{} << "_id" << from_account.view()["_id"].get_oid() << finalize, update_from.view());

	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_update_msg_claim(const Chain::message_xmax& msg)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::stream;

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		auto claim = msg.as<Basetypes::claim>();
		auto from_account = find_account(accounts, claim.account);

		asset token = asset::from_string(from_account.view()["main_token"].get_utf8().value.to_string());
		asset unstack_token = asset::from_string(from_account.view()["unstacking_token"].get_utf8().value.to_string());
		unstack_token -= claim.amount;
		token += claim.amount;

		document update_from{};
		update_from << "$set" << open_document
			<< "main_token" << token.to_string()
			<< "unstacking_token" << unstack_token.to_string()
			<< "updatedTime" << b_date{ now }
		<< close_document;

		accounts.update_one(document{} << "_id" << from_account.view()["_id"].get_oid() << finalize, update_from.view());
	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_update_msg_setcode(const Chain::message_xmax& msg)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::stream;

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		auto setcode = msg.as<Basetypes::setcode>();
		auto from_account = find_account(accounts, setcode.account);

		document update_from{};
		update_from << "$set" << open_document
			<< "abi" << bsoncxx::from_json(fc::json::to_string(setcode.code_abi))
			<< "updatedTime" << b_date{ now }
		<< close_document;

		accounts.update_one(document{} << "_id" << from_account.view()["_id"].get_oid() << finalize, update_from.view());
	}

	//--------------------------------------------------
	void mongodb_plugin_impl::_handle_finalized_block(const signed_block& block)
	{
				
		_check_block_state(block);

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });

		
		_insert_block(block);


	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_check_block_state(const signed_block& block)
	{
		auto blocks = mongo_client[mongodb_name][col_blocks_name]; // Blocks
		const auto prev_block_id = block.previous.str();
		const auto block_id = block.id();
		auto block_num = block.block_num();

		if (handled_block_num == 0)
		{
			//If started, check valid state for the last block
			if (started)
			{
				mongocxx::options::find opts;
				opts.sort(bsoncxx::from_json(R"foo({ "_id" : -1 })foo"));
				auto last_block = blocks.find_one({}, opts);
				if (!last_block) {
					//Release constraint temporary cause of the genesis block finalization which has not yet been finished
					//FC_THROW("No blocks found in the mongo db");
					return;
				}
				const auto id = last_block->view()["block_id"].get_utf8().value.to_string();
				if (id != prev_block_id) {
					FC_THROW("Can not find the expected previous block ${pid}! Found id: ${id}", ("pid", prev_block_id)("id", id));
				}
			}
			//There must be no block at all
			else
			{
				if(blocks.count(bsoncxx::from_json("{}")) > 0) {
					FC_THROW("Error! There already exist a block.");
				}

				FC_ASSERT(block_num < 2, "Expected start of block, instead received block_num: ${bn}", ("bn", block_num));			
				if (block_num == 1 && block.builder == Config::xmax_contract_name) {
					block_num = 0;
				}
			}
		}
	}


	//--------------------------------------------------
	void mongodb_plugin_impl::_insert_block(const signed_block& block)
	{
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder;


		stream::document block_doc{};		
		const auto block_id = block.id();
		const auto block_id_str = block_id.str();
		const auto prev_block_id = block.previous.str();
		auto block_num = block.block_num();
		
		  
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });

		auto block_size = fc::raw::pack_size(block);

		block_doc << "block_num" << b_int32{ static_cast<int32_t>(block_num) }
			<< "block_id" << block_id_str
			<< "prev_block_id" << prev_block_id
			<< "block_size" << b_int32{ static_cast<int32_t>(block_size) }
			<< "timestamp" << b_date{ std::chrono::milliseconds{ std::chrono::seconds{Chain::chain_timestamp::to_time_point(block.timestamp).sec_since_epoch()}} }
			<< "transaction_merkle_root" << block.trxs_mroot.str() 
			<< "builder_account_name" << block.builder.to_string();			
		
		_insert_block_transactions(block, block_doc);
		
		
	}


	//--------------------------------------------------
	inline void mongodb_plugin_impl::_insert_block_transactions(const signed_block& block, bsoncxx::builder::stream::document& block_doc)
	{
		
		using namespace bsoncxx::builder;
		using namespace bsoncxx::types;
		using namespace Xmaxplatform::Chain;

		auto trans_arr_ctx = block_doc << "transactions" << stream::open_array;

		mongocxx::options::bulk_write bulk_opts;
		bulk_opts.ordered(false);
		mongocxx::bulk_write bulk_trans{ bulk_opts };

		uint32_t trans_idx = 0;
		bool transactions_in_block = false;

		auto trans = mongo_client[mongodb_name][col_transactions_name];

		
		for (const auto& receipt : block.receipts)
		{
			string trx_id_str;

			if (receipt.trx.contains<transaction_package>()) {
				const auto& packed_trx = receipt.trx.get<transaction_package>();
				const auto& trx = packed_trx.body;
				const auto& id = trx.id();
				trx_id_str = id.str();

				if (_insert_single_transaction(trans_arr_ctx, block, trans_idx, trx, bulk_trans))
				{
					transactions_in_block = true;
				}
				++trans_idx;
			}
			else {
				const auto& trx_id = receipt.trx.get<xmax_type_transaction_id>();
				trx_id_str = trx_id.str();
				//TODO: record the id info
			}

			auto op_result = find_transaction(trans, trx_id_str);

		}


		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });

		auto blk_complete = trans_arr_ctx << stream::close_array
			<< "createdTime" << b_date{ now }
		<< stream::finalize;

		auto blocks = mongo_client[mongodb_name][col_blocks_name];
		const auto block_id = block.id();

		if (!blocks.insert_one(blk_complete.view())) {
			elog("Insert failed for the block ${bid}", ("bid", block_id));
		}

		
		if (transactions_in_block) {
			auto result = trans.bulk_write(bulk_trans);
			if (!result) {
				elog("Insert transactions failed for the block: ${bid}", ("bid", block_id));
			}
		}

		++handled_block_num;
	}


	//--------------------------------------------------
	template <class T>
	bool mongodb_plugin_impl::_insert_single_transaction(T& trans_arr_ctx, const signed_block& block, uint32_t trans_idx, 
		const Chain::signed_transaction& trans, mongocxx::bulk_write& bulk_trans)
	{
		using namespace bsoncxx::builder;
		using namespace bsoncxx::types;

		if (!account_filter.empty() && !_account_filter_include(trans.scope)) {
			return false;
		}

		auto oid = bsoncxx::oid{};
		trans_arr_ctx = trans_arr_ctx << oid;
		stream::document doc{};
		const auto trans_id = trans.id().str();
		auto trans_ctx = doc
			<< "_id" << oid
			<< "transaction_id" << trans_id
			<< "transaction_index" << b_int32{ static_cast<int32_t>(trans_idx) }
			<< "block_id" << block.id().str()
			<< "ref_block_num" << b_int32{ static_cast<int32_t>(trans.ref_block_num) }
			<< "ref_block_prefix" << trans.ref_block_prefix.str()
			<< "scope" << stream::open_array;

		for (const auto& account_name : trans.scope)
			trans_ctx = trans_ctx << account_name.to_string();

		trans_ctx = trans_ctx << stream::close_array
			<< "expiration" << b_date{ std::chrono::milliseconds{ std::chrono::seconds{ trans.expiration.sec_since_epoch() } } }
		<< "signatures" << stream::open_array;


		for (const auto& sig : trans.signatures) {
			trans_ctx = trans_ctx << fc::variant(sig).as_string();
		}

		trans_ctx = trans_ctx
			<< stream::close_array
			<< "messages" << stream::open_array;

		mongocxx::options::bulk_write bulk_opts;
		bulk_opts.ordered(false);
		mongocxx::bulk_write bulk_msgs{ bulk_opts };
		int32_t idx = 0;
		for (const auto& msg : trans.messages) {
			_insert_single_message(idx, trans_ctx, block, bulk_msgs, msg, trans);
			++idx;
		}

		auto msgs = mongo_client[mongodb_name][col_messages_name];
		const auto block_id = block.id();

		if (!trans.messages.empty()) {
			auto result = msgs.bulk_write(bulk_msgs);
			if (!result) {
				elog("Bulk message insert failed for block: ${bid}, transaction: ${trx}", ("bid", block_id)("trx", trans.id()));
			}
		}

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });

		auto complete_doc = trans_ctx << stream::close_array
			<< "createdTime" << b_date{ now }
		<< stream::finalize;
		mongocxx::model::insert_one insert_op{ complete_doc.view() };		
		bulk_trans.append(insert_op);
		return true;

	}


	//--------------------------------------------------
	template <class TRANS_CTX_TYPE>
	inline void mongodb_plugin_impl::_insert_single_message(int32_t idx, TRANS_CTX_TYPE& trans_ctx, const signed_block& block, 
		mongocxx::bulk_write& bulk_msgs, const Xmaxplatform::Basetypes::message& message, const Chain::signed_transaction& trans)
	{
		using namespace bsoncxx::builder;
		using namespace bsoncxx::types;
		using namespace bsoncxx::builder::basic;

		auto msg_oid = bsoncxx::oid{};
		trans_ctx = trans_ctx << msg_oid;

		auto msg_doc = bsoncxx::builder::basic::document{};
		msg_doc.append(kvp("_id", b_oid{ msg_oid }),
			kvp("message_id", b_int32{ idx }),
			kvp("transaction_id", trans.id().str()));

		msg_doc.append(kvp("authorization", [&message](bsoncxx::builder::basic::sub_array subarr) {
			for (const auto& auth : message.authorization) {
				subarr.append([&auth](bsoncxx::builder::basic::sub_document subdoc) {
					subdoc.append(kvp("account", auth.account.to_string()),
						kvp("permission", auth.authority.to_string()));
				});
			}
		}));

		msg_doc.append(kvp("handler_account_name", message.code.to_string()));
		msg_doc.append(kvp("type", message.type.to_string()));
		add_data(msg_doc, accounts, message);

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });

		msg_doc.append(kvp("createdTime", b_date{ now }));
		mongocxx::model::insert_one insert_msg{ msg_doc.view() };
		bulk_msgs.append(insert_msg);

		// xmax account update
		if (message.code == Config::xmax_contract_name) {
			try {
				update_account(message);
			}
			catch (fc::exception& e) {
				elog("Unable to update account ${e}", ("e", e.to_string()));
			}
		}
	}


	
	//--------------------------------------------------
	void mongodb_plugin_impl::initialize()
	{
		ilog("Mongo db impl initialize.");

				
		accounts = mongo_client[mongodb_name][col_accounts_name]; // Accounts
		bsoncxx::builder::stream::document doc{};
		
		bool firstTimeInit = accounts.count(doc.view()) == 0;
		if (firstTimeInit) {
			_init_mongodb_tables();
		}

	}

	//--------------------------------------------------
	void mongodb_plugin_impl::_init_mongodb_tables()
	{
		using namespace bsoncxx::types;

		ilog("Initialize the mongodb tables.");

		xmax_abi = Xmaxplatform::Native_contract::native_contract_chain_init::get_system_abi();

		bsoncxx::builder::stream::document doc{};

		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
		doc << "name" << Config::xmax_contract_name.to_string()
			<< "main_token" << asset(Config::initial_token_supply).to_string()
			<< "staked_token" << asset().to_string()
			<< "unstaking_token" << asset().to_string()
			<< "abi" << bsoncxx::from_json(fc::json::to_string(xmax_abi))
			<< "createdTime" << b_date{ now }
		<< "updatedTime" << b_date{ now };

		if (!accounts.insert_one(doc.view())) {
			elog("Failed to insert account ${n}", ("n", Config::xmax_contract_name.to_string()));
		}

		// Accounts indexes
		accounts.create_index(bsoncxx::from_json(R"foo({ "name" : 1 })foo"));

		// Transactions indexes
		auto trans = mongo_client[mongodb_name][col_transactions_name]; // Transactions
		trans.create_index(bsoncxx::from_json(R"foo({ "transaction_id" : 1 })foo"));

		// Messages indexes
		auto msgs = mongo_client[mongodb_name][col_messages_name]; // Messages
		msgs.create_index(bsoncxx::from_json(R"foo({ "message_id" : 1 })foo"));
		msgs.create_index(bsoncxx::from_json(R"foo({ "transaction_id" : 1 })foo"));

		// Blocks indexes
		auto blocks = mongo_client[mongodb_name][col_blocks_name]; // Blocks
		blocks.create_index(bsoncxx::from_json(R"foo({ "block_num" : 1 })foo"));
		blocks.create_index(bsoncxx::from_json(R"foo({ "block_id" : 1 })foo"));
	}



	//--------------------------------------------------
	void mongodb_plugin_impl::drop_tables()
	{
		ilog("Mongo db drop_tables.");

		accounts = mongo_client[mongodb_name][col_accounts_name]; // Accounts
		auto trans = mongo_client[mongodb_name][col_transactions_name]; // Transactions
		auto msgs = mongo_client[mongodb_name][col_messages_name]; // Messages
		auto blocks = mongo_client[mongodb_name][col_blocks_name]; // Blocks

		accounts.drop();
		trans.drop();
		msgs.drop();
		blocks.drop();
	}
#endif

	

	/*!
	 * Plugin interfaces
	 *
	 */
	//--------------------------------------------------
	mongodb_plugin::mongodb_plugin()
#ifdef MONGO_DB
		:db_impl(new mongodb_plugin_impl)
#endif // MONGO_DB
	{

	}

	//--------------------------------------------------
	mongodb_plugin::~mongodb_plugin()
	{
	}

	//--------------------------------------------------
	void mongodb_plugin::set_program_options(options_description& cli, options_description& cfg)
	{
#ifdef MONGO_DB

		cfg.add_options()
			("mongo-uri,m", bpo::value<std::string>(), "MongoDB connection URI string. Default db name is XMX and could be overwritten in URI.")
			("queue-length,q", bpo::value<uint32_t>()->default_value(DEFAULT_QUEUE_LENGTH), "The working queue length which handle blocks in mongodb plugin.");

#endif // MONGO_DB

	}	

	//--------------------------------------------------
	void mongodb_plugin::plugin_initialize(const variables_map& options)
	{
#ifdef MONGO_DB

		if (options.count("mongo-uri")) {
			ilog("Init mongodb_plugin plugin.");
			
			//Read config
			if (options.count("queue-length"))
			{
				db_impl->block_queue_length = options.at("queue-length").as<uint32_t>();
			}

			
			std::string uri_str = options.at("mongo-uri").as<std::string>();
			_connect_mongo_db(uri_str);

			if (db_impl->drop_tables_on_init)
			{
				db_impl->drop_tables();
				db_impl->drop_tables_on_init = false;
			}

			db_impl->initialize();

			db_impl->initialized = true;
		}
		else
		{
			wlog("Init mongodb_plugin failed, mongo-uri configuration could not be found!");
		}

#endif // MONGO_DB

	}

	//--------------------------------------------------
	void mongodb_plugin::_connect_mongo_db(const std::string& mongo_uri_str)
	{
#ifdef MONGO_DB
		mongocxx::uri uri = mongocxx::uri{ mongo_uri_str };
		db_impl->mongodb_name = uri.database();
		if (db_impl->mongodb_name.empty())
		{
			db_impl->mongodb_name = DEFAULT_MONGODB_NAME;
		}
		ilog("Connect to MongoDB:${uri}, Db name:${name}", ("uri", mongo_uri_str)("name", db_impl->mongodb_name));
		db_impl->mongo_client = mongocxx::client{ uri };
#endif // MONGO_DB
		
	}

	//--------------------------------------------------
	void mongodb_plugin::plugin_startup()
	{
#ifdef MONGO_DB
		if (db_impl->initialized)
		{
			db_impl->working_thread = boost::thread([this] { db_impl->handle_blocks(); });

			db_impl->started = true;
		}
#endif // MONGO_DB
	
	}

	//--------------------------------------------------
	void mongodb_plugin::plugin_shutdown()
	{
#ifdef MONGO_DB
		db_impl.reset();
#endif // MONGO_DB		
	}

	//--------------------------------------------------
	void mongodb_plugin::drop_tables_on_init()
	{
#ifdef MONGO_DB
		if (db_impl->initialized) 
		{
			elog("Error! Call drop_tables_on_startup after mongodb_plugin initialized!");
		}
		else 
		{
			db_impl->drop_tables_on_init = true;
		}		

#endif // MONGO_DB

	}


	//--------------------------------------------------
	void mongodb_plugin::finalize_block(const Chain::signed_block& block)
	{
#ifdef MONGO_DB
		db_impl->finalize_block(block);
#endif // MONGO_DB
	}

	} // namespace Xmaxplatform
