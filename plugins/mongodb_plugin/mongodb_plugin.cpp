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
	using Xmaxplatform::Basetypes::permission_name;

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


		void initialize();
		void drop_tables();
		void handle_blocks();		

	
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

		Basetypes::abi mongodb_plugin_impl::xmax_abi;
		mongocxx::collection accounts;

		boost::thread working_thread;
		std::queue<signed_block> block_queue;
		uint32_t block_queue_length;
		boost::mutex block_queue_mutex;	
		boost::condition_variable condtion;
		boost::atomic<bool> work_done{ false };
	};


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
				//	process_irreversible_block(block);
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

	}

	//--------------------------------------------------
	void mongodb_plugin_impl::initialize()
	{
		ilog("Mongo db impl initialize.");

		using namespace bsoncxx::types;


		xmax_abi = Xmaxplatform::Native_contract::native_contract_chain_init::xmax_contract_abi();

		accounts = mongo_client[mongodb_name][col_accounts_name]; // Accounts
		bsoncxx::builder::stream::document doc{};
		
		bool firstTimeInit = accounts.count(doc.view()) == 0;
		if (firstTimeInit) {
			auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::microseconds{ fc::time_point::now().time_since_epoch().count() });
			doc << "name" << Config::xmax_contract_name.to_string()
				<< "xmx_token" << asset(Config::initial_token_supply).to_string()
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
