/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */


#include <blockchain_types.hpp>
#include <block.hpp>
#include <blockchain_exceptions.hpp>
#include <basechain.hpp>
#include <forkchain.hpp>
#include <chain_utils.hpp>
#include <chain_init.hpp>
#include <chain_xmax.hpp>
#include <xmax_voting.hpp>
#include <authoritys_utils.hpp>

#include <rand.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/uint128.hpp>
#include <fc/crypto/digest.hpp>
#include <fc/exception/exception.hpp>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/map.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <chrono>

#include <objects/authority_object.hpp>
#include <objects/transaction_object.hpp>
#include <objects/block_summary_object.hpp>
#include <objects/account_object.hpp>
#include <objects/vote_objects.hpp>
#include <objects/resource_token_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/builder_object.hpp>
#include <objects/global_status_objects.hpp>
#include <objects/erc20_token_object.hpp>
#include <objects/erc721_token_object.hpp>
#include <objects/erc721_token_account_object.hpp>

#include <transaction_context_xmax.hpp>
#include <pending_block.hpp>
#include <chain_stream.hpp>

#include <vm_xmax.hpp>

#include <abi_serializer.hpp>



namespace Xmaxplatform { namespace Chain {

	class chain_context
	{
	public:
		typedef pair<account_name, Basetypes::name> handler_key;
		optional<pending_block>				building_block;
		block_pack_ptr						block_head;
		chain_stream						chain_log;
		chain_xmax::xmax_config				config;
		database							block_db;
		forkdatabase						fork_db;
		Chain::xmax_type_block_num			last_irreversible_block_num = 0;

		const uint32_t                   pending_txn_depth_limit;
		uint64_t                         skip_flags = 0;

		map< account_name, map<handler_key, msg_handler> >  message_handlers;

		vector<transaction_request_ptr>         pending_transactions;


		chain_context(const chain_xmax::xmax_config& _config, uint32_t _txn_depth_limit)
			: config(_config)
			, chain_log(_config.block_log_dir)
			, pending_txn_depth_limit(_txn_depth_limit)
			, block_db(config.block_memory_dir,
				config.open_flag ? database::read_only : database::read_write,
				config.shared_memory_size)
			, fork_db(config.block_memory_dir)
		{
			//--------------------------------------
//#pragma message("-------------------------------------- skip some for test. --------------------------------------") 
//			skip_flags = skip_flags | Config::skip_confirmation;
			//-----------------------------------------
		}
		~chain_context()
		{
			building_block.reset();
			block_db.flush();
			fork_db.close();
		}

		//template<typename Function>
		//auto with_applying_block(Function&& f) -> decltype((*((Function*)nullptr))()) {
		//	auto on_exit = fc::make_scoped_exit([this]() {
		//		currently_applying_block = false;
		//	});
		//	currently_applying_block = true;
		//	return f();
		//}

		//template<typename Function>
		//auto with_skip_flags(uint64_t flags, Function&& f) -> decltype((*((Function*)nullptr))())
		//{
		//	auto old_flags = skip_flags;
		//	auto on_exit = fc::make_scoped_exit([&]() {skip_flags = old_flags; });
		//	skip_flags = flags;
		//	return f();
		//}

	};


        void chain_xmax::setup_data_indexes() {
            _context->block_db.add_index<account_index>();
			_context->block_db.add_index<permission_index>();

            _context->block_db.add_index<key_value_index>();
            _context->block_db.add_index<keystr_value_index>();
            _context->block_db.add_index<key128x128_value_index>();
            _context->block_db.add_index<key64x64x64_value_index>();

			_context->block_db.add_index<transaction_multi_index>();
			_context->block_db.add_index<block_summary_multi_index>();

            _context->block_db.add_index<static_config_multi_index>();
            _context->block_db.add_index<dynamic_states_multi_index>();
            _context->block_db.add_index<xmx_token_multi_index>();

			_context->block_db.add_index<voter_info_index>();
			_context->block_db.add_index<builder_info_index>();
			_context->block_db.add_index<builder_multi_index>();
			_context->block_db.add_index<resource_token_multi_index>();

			_context->block_db.add_index<global_trx_status_index>();
			_context->block_db.add_index<global_msg_status_index>();

			_context->block_db.add_index<erc20_token_multi_index>();
			_context->block_db.add_index<erc721_token_multi_index>();
			_context->block_db.add_index<erc721_token_account_multi_index>();

        }

        void chain_xmax::initialize_chain(chain_init& initer)
        { 
			bool bfirst_init = !_context->block_db.find<static_config_object>();			
			
			_context->fork_db.bind_irreversible(this, &chain_xmax::on_irreversible);
			if (bfirst_init) 
			{
				first_initialize(initer);
			}
			else
			{
				initialize_impl(initer);
			}
        }

		void chain_xmax::first_initialize(chain_init& initer)
		{
			try {
				ilog("chain_xmax first initialize.");
				const fc::time_point init_point = initer.get_chain_init_time();;
				const chain_timestamp init_stamp = chain_timestamp::from(init_point);
				//const chain_timestamp pre_stamp = init_stamp - chain_timestamp::create(1);


				_start_first_build(init_stamp);

				// block genesis db.

				// Create global properties
				_context->block_db.create<static_config_object>([&](static_config_object &p) {
					p.setup = initer.get_blockchain_setup();
					p.current_builders = _context->building_block->pack->current_builders;
				});

				_context->block_db.create<dynamic_states_object>([&](dynamic_states_object &p) {
					p.head_block_number = 0;
					p.head_block_id = xmax_type_block_id();
					p.state_time = init_stamp.time_point();
					p.total_slot = 0;
					p.block_builder = _context->building_block->pack->bld_info.builder_name;

					p.round_slot = _context->building_block->pack->round_slot;
				});

				_context->block_db.create<global_trx_status_object>([](global_trx_status_object &obj) {
					obj.counter = 0;
				});
				_context->block_db.create<global_msg_status_object>([](global_msg_status_object &obj) {
					obj.counter = 0;
				});

				for (int i = 0; i < 0x10000; i++)
					_context->block_db.create<block_summary_object>([&](block_summary_object&) {});

				// make genesis message.
				{			
					auto messages = initer.prepare_data(*this, _context->block_db);

					signed_transaction signed_trx;

					std::set<account_name> scopes;

					std::for_each(messages.begin(), messages.end(), [&](const message_data& m) {
						signed_trx.messages.push_back(m.msg);
						scopes.insert(m.scopes.begin(), m.scopes.end());
					});

					signed_trx.scope.assign(scopes.begin(), scopes.end());

					transaction_request_ptr request = std::make_shared<transaction_request>(signed_trx);
					apply_transaction_impl(request);
				}

				_generate_next_builders();

				_generate_block();
				_sign_block(Config::xmax_build_private_key);
				_final_block();

				Chain::xmax_type_block_num block_num = _context->block_head->block_num;
				_context->chain_log.append_block(_context->block_head->block);				
				_context->building_block->push_db();
				_context->building_block.reset();
				_context->block_db.commit(block_num);
				_context->block_db.set_revision(block_num);

			} FC_CAPTURE_AND_RETHROW()
		}

		void chain_xmax::initialize_impl(chain_init& initer)
		{
			ilog("chain_xmax first initialize.");
			block_pack_ptr pack_head = _context->fork_db.get_head();
			if (!pack_head)
			{
				wlog("No head block in fork db, try to fix...");

				signed_block_ptr head = _context->chain_log.get_head();
				pack_head = std::make_shared<block_pack>();

				const auto& static_config = get_static_config();
				const auto& dynamic_states = get_dynamic_states();

				pack_head->refresh(head, static_config.current_builders, static_config.next_builders, dynamic_states.round_slot, true, true, true);

			}

			_context->block_head = pack_head;

			int64_t revision = _context->block_db.revision();

			ilog("block db revision: ${revision}", ("revision", revision));

			int64_t block_num = (int64_t)_context->block_head->block_num;

			FC_ASSERT(block_num <= revision, "Error: block_num > db revision.");

			if (block_num != revision)
			{
				wlog("block_num != revision, try to roll-back the db revision");
				_context->block_db.undo_all();
				_context->block_db.set_revision(block_num);
			}
			_context->last_irreversible_block_num = revision;
		}

        chain_xmax::chain_xmax(chain_init& init, const xmax_config& config, const finalize_block_func& finalize_func)
		: _context(new chain_context(config, 1000)) {

            setup_data_indexes();
            init.register_handlers(*this, _context->block_db);

			initialize_chain(init);

			if (finalize_func) {
				on_finalize_block.connect(*finalize_func);
			}
        }

        chain_xmax::~chain_xmax() {

        }

        const static_config_object& chain_xmax::get_static_config()const {
            return _context->block_db.get<static_config_object>();
        }

        const dynamic_states_object& chain_xmax::get_dynamic_states() const {
            return _context->block_db.get<dynamic_states_object>();
        }

        time chain_xmax::head_block_time() const {
			return head_block_timestamp().time_point();
        }

		chain_timestamp chain_xmax::head_block_timestamp() const
		{
			return _context->block_head->new_header.timestamp;
		}

		uint32_t chain_xmax::head_block_num() const
		{
			return _context->block_head->block_num;
		}

		chain_timestamp chain_xmax::building_block_timestamp() const
		{
			FC_ASSERT(_context->building_block, "no pending block");
			return _context->building_block->pack->new_header.timestamp;
		}

		uint32_t chain_xmax::last_irreversible_block_num() const
		{
			return _context->last_irreversible_block_num;
		}

		Xmaxplatform::Chain::xmax_type_block_id chain_xmax::head_block_id() const
		{
			return  _context->block_head->block_id;
		}

		signed_block_ptr chain_xmax::block_from_num(uint32_t num) const
		{
			try {
				block_pack_ptr pack = _context->fork_db.get_main_block_by_num(num);
				if (pack)
				{
					return pack->block;
				}
				signed_block_ptr block = _context->chain_log.read_by_num(num);
				uint32_t blockNum = block->block_num();
				FC_ASSERT(blockNum == num, "Wrong block was read from block log.");

				return block;

			} FC_LOG_AND_RETHROW()
		}
		signed_block_ptr chain_xmax::block_from_id(xmax_type_block_id id) const
		{
			try {
				// empty now. fill later.
				return signed_block_ptr();
			}FC_LOG_AND_RETHROW()
		}
		xmax_type_block_id chain_xmax::block_id_from_num(uint32_t num) const
		{
			if (auto ptr = block_from_num(num))
			{
				return ptr->id();
			}
			return empty_chain_id;
		}

		signed_block_ptr chain_xmax::head_block() const
		{
			return head_block_pack()->block;
		}
		block_pack_ptr chain_xmax::head_block_pack() const
		{
			return _context->block_head;
		}
		const mapped_builder_rule& _get_builder_rule(const static_config_object& config, uint32_t order_slot)
		{

			return utils::select_builder_rule(config.current_builders, config.next_builders, order_slot);
		}

		const builder_info& _get_builder(const static_config_object& config, uint32_t index)
		{
			return utils::select_builder(config.current_builders, config.next_builders, index);
		}

		const builder_info& chain_xmax::get_block_builder(uint32_t delta_slot) const
		{
			asset(delta_slot >= 0);
			const dynamic_states_object& states = get_dynamic_states();

			uint32_t order_slot = states.round_slot + delta_slot;

			return get_order_builder(order_slot);
		}

		const builder_info& chain_xmax::get_order_builder(uint32_t order_slot) const
		{
			asset(order_slot >= 0);
			const static_config_object& config = get_static_config();
			return _get_builder(config, order_slot);
		}

		const builder_object* chain_xmax::find_builder_object(account_name builder_name) const
		{
			return _context->block_db.find<builder_object, by_owner>(builder_name);
		}

		uint32_t chain_xmax::get_delta_slot_at_time(chain_timestamp when) const
		{
			return utils::get_delta_slot_at_time(head_block_timestamp(), when);
		}

		uint32_t chain_xmax::get_order_slot_at_time(chain_timestamp when) const
		{
			uint32_t delta = get_delta_slot_at_time(when);

			const dynamic_states_object& states = get_dynamic_states();

			return states.round_slot + delta;
		}

		const mapped_builder_rule& chain_xmax::get_verifiers_by_order(uint32_t order_slot) const
		{
			return _get_builder_rule(get_static_config(), order_slot);
		}

        chain_timestamp chain_xmax::get_delta_slot_time(uint32_t delta_slot) const
        {
			return utils::get_delta_slot_time(head_block_timestamp(), delta_slot);

        }
		const Basechain::database& chain_xmax::get_database() const
		{ 
			return _context->block_db;
		}
		Basechain::database& chain_xmax::get_mutable_database()
		{ 
			return _context->block_db;
		}

		vector<char> chain_xmax::message_to_binary(name code, name type, const fc::variant& obj)const
		{
			try {
				const auto& code_account = _context->block_db.get<account_object, by_name>(code);
				Xmaxplatform::Basetypes::abi abi;
				if (Basetypes::abi_serializer::to_abi(code_account.abi, abi)) {
					Basetypes::abi_serializer abis(abi);
					return abis.variant_to_binary(abis.get_action_type(type), obj);
				}
				return vector<char>();
			} FC_CAPTURE_AND_RETHROW((code)(type)(obj))
		}

		fc::variant chain_xmax::message_from_binary(name code, name type, const vector<char>& bin) const
		{
			const auto& code_account = _context->block_db.get<account_object, by_name>(code);
			Xmaxplatform::Basetypes::abi abi;
			if (Basetypes::abi_serializer::to_abi(code_account.abi, abi)) {
				Basetypes::abi_serializer abis(abi);
				return abis.binary_to_variant(abis.get_action_type(type), bin);
			}
			return fc::variant();
		}


		//--------------------------------------------------
		fc::variant chain_xmax::event_from_binary(name code, type_name tname, const vector<char>& bin) const {
			const auto& code_account = _context->block_db.get<account_object, by_name>(code);
			Xmaxplatform::Basetypes::abi abi;
			if (Basetypes::abi_serializer::to_abi(code_account.abi, abi)) {
				Basetypes::abi_serializer abis(abi);
				return abis.binary_to_variant(tname, bin);
			}
			return fc::variant();
		}



		transaction_response_ptr chain_xmax::push_transaction(const signed_transaction& trx, uint32_t skip /*= skip_nothing*/)
		{
			transaction_request_ptr request = std::make_shared<transaction_request>(trx);
			return push_transaction(request);

		}

		transaction_response_ptr chain_xmax::push_transaction(transaction_request_ptr request)
		{
			if (check_trx(request))
			{
				if (!_context->building_block.valid())
				{
					_context->pending_transactions.push_back(request);
					return make_response();
				}

				return apply_transaction_impl(request);
			}
			else
			{
				return make_response();
			}
		}

		transaction_response_ptr chain_xmax::apply_transaction(transaction_request_ptr request)
		{
			FC_ASSERT(_context->building_block.valid(), "not transaction apply time.");
			if (check_trx(request))
			{
				return apply_transaction_impl(request);
			}
			else
			{
				return make_response();
			}
		}

		transaction_response_ptr chain_xmax::apply_transaction_impl(transaction_request_ptr request)
		{
			try {
				transaction_response_ptr response;
				transaction_context_xmax Impl(*this, request->signed_trx);

				utils::check_authorization(request->signed_trx.messages, request->signed_trx.get_signature_keys(_context->config.chain_id));


				Impl.exec();

				record_transaction(request->signed_trx);

				response = Impl.get_response();

				response->receipt = apply_transaction_receipt(request->signed_trx);

				Impl.squash();
				fc::move_append(_context->building_block->message_receipts, std::move(Impl.msg_receipts));
				_context->building_block->pack->transactions.push_back(request);
				return response;
			}
			catch (const fc::exception& e) {
				wlog("${details}", ("details", e.to_detail_string()));

				return make_response(e);
			}
		}

		transaction_response_ptr chain_xmax::make_response() const
		{
			return std::make_shared<transaction_response>();
		}
		transaction_response_ptr chain_xmax::make_response(const fc::exception& e) const
		{
			transaction_response_ptr response = std::make_shared<transaction_response>();
			response->error = e;
			response->error_ptr = std::current_exception();

			return response;
		}
		transaction_receipt& chain_xmax::apply_transaction_receipt(const signed_transaction& trx)
		{
			block_pack& block_pk = *_context->building_block->pack;

			block_pk.block->receipts.emplace_back(transaction_receipt(transaction_package(trx)));

			transaction_receipt& receipt = block_pk.block->receipts.back();
			uint64_t idx = 0;
			{
				const auto& obj = _context->block_db.get<global_trx_status_object>();
				_context->block_db.modify(obj, [&](auto& mrs) {
					++mrs.counter;
				});
				idx = obj.counter;
			}

			receipt.receipt_idx = idx;

			receipt.result = transaction_receipt::applied;
			return receipt;
		}
		
		void chain_xmax::push_confirmation(const block_confirmation& conf)
		{
			process_confirmation(conf);
		}

		//--------------------------------------------------
		Xmaxplatform::Chain::flat_set<Xmaxplatform::Chain::public_key_type> chain_xmax::get_required_keys(const signed_transaction& transaction, const flat_set<public_key_type>& candidateKeys) const
		{
			// Todo: Authentication is to be supported later. Return keys directly now.
			return flat_set<Xmaxplatform::Chain::public_key_type>(candidateKeys);			
		}

		void chain_xmax::build_block(
                chain_timestamp when,
				const private_key_type& sign_private_key
        ) { 

			auto exec_start = std::chrono::high_resolution_clock::now();

			_abort_build();

			_start_build(when);

			for (auto request : _context->pending_transactions)
			{
				apply_transaction(request);
			}
			_context->pending_transactions.clear();
			_generate_block();

			_sign_block(sign_private_key);

			_final_block();

			auto exec_stop = std::chrono::high_resolution_clock::now();
			auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_stop - exec_start);

			const auto& new_block = _context->building_block->pack->block;

			ilog("${builder} generate block #${num}  at ${time}, exectime_ms=${extm}",
				("builder", new_block->builder)
				("time", new_block->timestamp)
				("num", new_block->block_num())
				("extm", exec_ms.count())
			);

			_push_fork();
			_commit_block();
		}

		void chain_xmax::confirm_block(const signed_block_ptr next_block)
		{
			_apply_block(next_block, true);
		}

		void chain_xmax::_apply_block(signed_block_ptr block, bool fork)
		{
			_validate_block_desc(block);

			vector<transaction_request_ptr> transactions;

			for (auto trx : block->receipts)
			{
				if (trx.trx.contains<transaction_package>())
				{
					const auto& package = trx.trx.get<transaction_package>();
					transactions.push_back(std::make_shared<transaction_request>(package));
				}
				else
				{
					XMAX_ASSERT(false, block_validate_exception, "encountered unexpected package type.");
				}
			}

			auto exec_start = std::chrono::high_resolution_clock::now();
			_abort_build();

			_start_build(block->timestamp);

			for (auto request : transactions)
			{
				apply_transaction(request);
			}

			_generate_block();

			_validate_block(block);

			_final_block();

			if (fork)
			{
				_push_fork();
			}

			_commit_block();
		}

		void chain_xmax::_pop_block()
		{
			auto previous = _context->fork_db.get_block(_context->block_head->prev_id());

			_context->fork_db.change_main_chain_flag(_context->block_head->block_id, false);

			_context->block_head = previous;
		}

		void chain_xmax::_check_fork()
		{
			auto new_head = _context->fork_db.get_head();

			while (!new_head->main_chain)
			{
				fetch_branch branches = _context->fork_db.fetch_branch_from_fork(_context->block_head->block_id, new_head->block_id);
				try
				{
					branch_blocks& origin_branch = branches.first;

					branch_blocks::const_reverse_iterator it = origin_branch.rbegin();
					while (it != origin_branch.rend())
					{
						FC_ASSERT((*it)->block_id == _context->block_head->block_id);
						_pop_block();
						++it;
					}
				}
				catch (const fc::exception& e)
				{
					_context->fork_db.remove_chain(new_head->block_id);
					new_head = _context->fork_db.get_head();
					continue;
				}

				branch_blocks& new_branch = branches.second;
				for ( auto rit = new_branch.rbegin(); rit != new_branch.rend(); ++rit)
				{
					try
					{
						_apply_block((*rit)->block, false);
						_context->fork_db.change_main_chain_flag((*rit)->block_id, true);
					}
					catch (const fc::exception& e)
					{
						_context->fork_db.remove_chain((*rit)->block_id);
						ilog("bad forks ${e}", ("e", e.to_detail_string()));
						break;
					}
				}

			}
		}

		void chain_xmax::push_block(const signed_block_ptr block)
		{
			_context->fork_db.add_block(block);
			_check_fork();
		}

		void chain_xmax::broadcast_confirmation(account_name account, const private_key_type& validate_private_key, broadcast_confirm_func confirm_func)
		{
			_broadcast_confirmation(_context->building_block->pack->block_id, account, validate_private_key, confirm_func);
		}

		void chain_xmax::_abort_build()
		{
			if (_context->building_block)
			{
				_context->building_block.reset();
			}
		}

		void chain_xmax::_start_first_build(chain_timestamp when)
		{
			builder_info xmxinfo(Config::xmax_contract_name, Config::xmax_build_public_key);
			xmax_builder_infos list;
			list.push_back(xmxinfo);
			builder_rule rule;
			rule.set_builders(list, 1);

			_context->building_block = _context->block_db.start_undo_session(true);

			// start build.
			_context->building_block->pack = std::make_shared<block_pack>();
			_context->building_block->pack->init_default(when, xmxinfo, rule);

			_context->block_head = _context->building_block->pack;
		}

		void chain_xmax::_start_build(chain_timestamp when)
		{
			FC_ASSERT(!_context->building_block);
			//check properties.
			FC_ASSERT(head_block_time() < when.time_point(), "block must be generated at a timestamp after the head block time");

			auto pending_undo = fc::make_scoped_exit([&]() {
				_context->building_block.reset();
			});

			_context->building_block = _context->block_db.start_undo_session(true);

			try {

				_context->building_block->pack = std::make_shared<block_pack>();

				block_pack& pack = *_context->building_block->pack;

				_context->building_block->pack->init_by_pre_pack(*_context->block_head, when, true);

				_generate_next_builders();
				
				pending_undo.cancel();

			} FC_CAPTURE_AND_RETHROW((when))
		}

		void chain_xmax::_generate_next_builders()
		{				
			if (_context->building_block->pack->new_round)
			{
				xmax_builder_infos new_builders = Native_contract::xmax_voting::next_round(_context->block_db);

				uint32_t new_version = _context->building_block->pack->current_builders.version + 1;
				builder_rule newrule;
				newrule.set_builders(new_builders, new_version);

				_context->building_block->pack->set_next_builders(newrule);

				std::stringstream namestream;
				for (const builder_info& it : new_builders)
				{
					namestream << it.builder_name.to_string() << ",";
				}
				fc::mutable_variant_object capcture;
				capcture.set("builders", namestream.str());

				ilog("next round: ${builders}", (capcture));
			}
		}

        void chain_xmax::_generate_block() {

			FC_ASSERT(_context->building_block);
            try {

				signed_block_header& building_header = _context->building_block->pack->new_header;
				signed_block& building_block = *_context->building_block->pack->block;
				// merkle

				const auto& trxs = building_block.receipts;

				std::vector<xmax_type_summary> hashs;
				hashs.reserve(trxs.size());
				for (const auto& t : trxs)
				{
					hashs.emplace_back(t.cal_digest());
				}

				building_header.trxs_mroot = utils::cal_merkle(hashs);

            } FC_CAPTURE_AND_RETHROW( (_context->building_block->pack->new_header.builder) )
		}

		void chain_xmax::_sign_block(const private_key_type& sign_private_key)
		{
			try {
				signed_block_header& building_header = _context->building_block->pack->new_header;
				building_header.sign(sign_private_key);
				_context->building_block->pack->block_id = building_header.id();
				_context->building_block->pack->validated = true;
			} FC_CAPTURE_AND_RETHROW((_context->building_block->pack->new_header.builder))
		}

		void chain_xmax::_validate_block_desc(signed_block_ptr block)
		{
			FC_ASSERT(_context->block_head->block_id == block->previous, "head block id != previous id of next mblock");

			FC_ASSERT(_context->block_head->block_num + 1 == block->block_num(), "head block number + 1 != next block number");

			//FC_ASSERT(block->transaction_merkle_root == block->calculate_merkle_root(), "action merkle root does not match");

			const builder_object* builder = find_builder_object(block->builder);

			FC_ASSERT(block->is_signer_valid(builder->signing_key), "bad block.");
		}

		void chain_xmax::_validate_block(const signed_block_ptr next_block)
		{
			try {

				//set builder_signature first.
				_context->building_block->pack->new_header.builder_signature = next_block->builder_signature;

				const xmax_type_block_id id = next_block->id();

				const xmax_type_block_id v_id = _context->building_block->pack->new_header.id();
			// validate id
				FC_ASSERT(v_id == id, "bad block");

				_context->building_block->pack->block_id = v_id;

				_context->building_block->pack->validated = true;

			} FC_CAPTURE_AND_RETHROW((next_block))
		}

		void chain_xmax::_broadcast_confirmation(xmax_type_block_id id, account_name account, const private_key_type& validate_private_key, broadcast_confirm_func confirm_func)
		{
			block_confirmation conf;
			conf.block_id = id;
			conf.verifier = account;

			conf.sign(validate_private_key);

			push_confirmation(conf);

			confirm_func(conf);
		}

		void chain_xmax::_make_fianl_block()
		{
			signed_block_header& building_header = _context->building_block->pack->new_header;
			// make final block from block pack.
			_context->building_block->pack->block = std::make_shared<signed_block>();

			signed_block_header* final_block = _context->building_block->pack->block.get();

			(*final_block) = building_header;
		}

		void chain_xmax::_final_block()
		{
			_make_fianl_block();

			_update_final_state(_context->building_block->pack);

			block_summary(*_context->building_block->pack->block);
		}

		void chain_xmax::_push_fork()
		{
			FC_ASSERT(_context->building_block);
			const auto pack = _context->building_block->pack;
			try {

				_context->fork_db.add_block(pack);
				_context->block_head = _context->fork_db.get_head();
				FC_ASSERT(pack == _context->block_head, "error new head in fork database");
			} FC_CAPTURE_AND_RETHROW((pack->block_num))
		}

		void chain_xmax::_commit_block() {

			FC_ASSERT(_context->building_block);
			const auto& new_block = _context->building_block->pack->block;

			try {

				_context->building_block->push_db();
				_context->building_block.reset();

			} FC_CAPTURE_AND_RETHROW((new_block->block_num()))
		
		}

		void chain_xmax::_update_final_state(const block_pack_ptr& pack)
		{		
			const signed_block& last_block = *pack->block;

			const dynamic_states_object& dy_state = get_dynamic_states();

			chain_timestamp current_block_time = last_block.timestamp;

			uint32_t delta_slot = get_delta_slot_at_time(current_block_time);

			uint32_t total_slot = dy_state.total_slot + delta_slot;


			// missed_block
			uint32_t missed_blocks = delta_slot - 1;
			// info of miss builders.
			if (missed_blocks > 0)
			{
				uint32_t fixed_missed = missed_blocks % Config::blocks_per_round;
				uint32_t pre_slot = dy_state.round_slot + 1;
				for (int i = 0; i < fixed_missed; ++i)
				{
					int miss_slot = pre_slot + i;
					account_name miss_name = get_order_builder(miss_slot).builder_name;
					if (const builder_object* builder_obj = find_builder_object(miss_name))
					{
						uint64_t last_miss = builder_obj->total_missed;
						_context->block_db.modify(*builder_obj, [&](builder_object& obj) {
							obj.total_missed = last_miss + 1;
						});
					}
				}
			}


			// store next builder list.
			if (pack->new_round)
			{
				FC_ASSERT(last_block.next_builders.valid());
				update_or_create_builders(*last_block.next_builders);

				const static_config_object& static_config = get_static_config();
				_context->block_db.modify(static_config, [&](static_config_object& obj) {
					obj.current_builders = pack->current_builders;
					obj.next_builders = pack->new_builders;
				});
			}		

			// update builder info
			if (const builder_object* builder_obj = find_builder_object(last_block.builder))
			{
				_context->block_db.modify(*builder_obj, [&](builder_object& obj) {
					obj.last_block_time = last_block.timestamp.time_point();
				});
			}


            // update dynamic states
            _context->block_db.modify( dy_state, [&]( dynamic_states_object& dgp ){
                dgp.head_block_number = last_block.block_num();
                dgp.head_block_id = last_block.id();
                dgp.state_time = last_block.timestamp.time_point();
                dgp.block_builder = last_block.builder;
				//dgp.round_begin_time = round_begin_time;
				dgp.total_slot = total_slot;
				dgp.round_slot = pack->round_slot;
            });

			on_finalize_block(last_block);
				
        }

		void chain_xmax::_irreversible_block(const block_pack_ptr& pack)
		{
			uint32_t block_num = pack->block->block_num();

			_context->last_irreversible_block_num = block_num;

			_context->chain_log.append_block(pack->block);

			_context->block_db.commit(block_num);
			_context->block_db.flush();
		}

		void chain_xmax::on_irreversible(block_pack_ptr pack)
		{
			auto pre_block = _context->chain_log.get_head();

			FC_ASSERT(pack->block_num - 1 == pre_block->block_num(), "error block", ("new block number", pack->block_num)("pre block number", pre_block->block_num()));
			FC_ASSERT(pack->block->previous == pre_block->id(), "new block doesn't link to pre block head");

			_irreversible_block(pack);
		}

        void chain_xmax::set_message_handler( const account_name& contract, const account_name& scope, const action_name& action, msg_handler v ) {
			_context->message_handlers[contract][std::make_pair(scope,action)] = v;
        }

		msg_handler chain_xmax::find_message_handler(const account_name& contract, const account_name& scope)
		{
			/// context.code => the execution namespace
			/// message.code / message.type => Event
			auto contract_handlers_itr = _context->message_handlers.find(contract);
			if (contract_handlers_itr != _context->message_handlers.end()) {
				auto message_handler_itr = contract_handlers_itr->second.find({ contract, scope });
				if (message_handler_itr != contract_handlers_itr->second.end()) {				
					return message_handler_itr->second;
				}
			}

			return msg_handler();
		}

		void chain_xmax::process_confirmation(const block_confirmation& conf)
		{
			_context->fork_db.add_confirmation(conf, _context->skip_flags);
		}

		void chain_xmax::validate_uniqueness(const Chain::signed_transaction& trx)const {
			if (!should_check_for_duplicate_transactions()) return;

			auto transaction = _context->block_db.find<transaction_object, by_trx_id>(trx.id());
			XMAX_ASSERT(transaction == nullptr, tx_duplicate, "Transaction is not unique");
		}

		void chain_xmax::record_transaction(const Chain::signed_transaction& trx) {
			//Insert transaction into unique transactions database.
			_context->block_db.create<transaction_object>([&](transaction_object& transaction) {
				transaction.trx_id = trx.id(); /// TODO: consider caching ID
				transaction.expiration = trx.expiration;
			});
		}

		void chain_xmax::validate_tapos(const transaction& trx)const {
			if (!should_check_tapos()) return;

			const auto& tapos_block_summary = _context->block_db.get<block_summary_object>((uint16_t)trx.ref_block_num);

			//Verify TaPoS block summary has correct ID prefix, and that this block's time is not past the expiration
			XMAX_ASSERT(transaction_verify_reference_block(trx, tapos_block_summary.block_id), transaction_exception,
				"Transaction's reference block did not match. Is this transaction from a different fork?",
				("tapos_summary", tapos_block_summary));
		}

		void chain_xmax::validate_referenced_accounts(const transaction& trx) const
		{
			for (const auto& scope : trx.scope)
				require_account(scope);
			for (const auto& msg : trx.messages) {
				require_account(msg.code);
				for (const auto& auth : msg.authorization)
					require_account(auth.account);
			}
		}

		void chain_xmax::validate_expiration(const transaction& trx) const
		{
			try {
				fc::time_point now = head_block_time();
				//const blockchain_configuration& chain_configuration = get_static_config().setup;

				static const int64_t max_trx_lifetime = 600;//chain_configuration.max_trx_lifetime

				XMAX_ASSERT(fc::time_point(trx.expiration) <= now + fc::seconds(max_trx_lifetime),
					transaction_exception, "Transaction expiration is too far in the future",
					("trx.expiration", trx.expiration)("now", now)
					("max_til_exp", max_trx_lifetime));

				XMAX_ASSERT(now <= trx.expiration, transaction_exception, "Transaction is expired",
					("now", now)("trx.exp", trx.expiration));
			} FC_CAPTURE_AND_RETHROW((trx))
		}

		void chain_xmax::validate_scope(const transaction& trx) const
		{
			XMAX_ASSERT(trx.scope.size() + trx.read_scope.size() > 0, transaction_exception, "No scope specified by transaction");
			for (uint32_t i = 1; i < trx.scope.size(); ++i)
				XMAX_ASSERT(trx.scope[i - 1] < trx.scope[i], transaction_exception, "Scopes must be sorted and unique");
			for (uint32_t i = 1; i < trx.read_scope.size(); ++i)
				XMAX_ASSERT(trx.read_scope[i - 1] < trx.read_scope[i], transaction_exception, "Scopes must be sorted and unique");

			vector<Basetypes::account_name> intersection;
			std::set_intersection(trx.scope.begin(), trx.scope.end(),
				trx.read_scope.begin(), trx.read_scope.end(),
				std::back_inserter(intersection));
			FC_ASSERT(intersection.size() == 0, "a transaction may not redeclare scope in read_scope");
		}

		bool chain_xmax::check_trx(const transaction_request_ptr& request) const
		{
			try {

				XMAX_ASSERT(request->signed_trx.messages.size() > 0, transaction_exception, "A transaction must have at least one message");
				validate_scope(request->signed_trx);
				validate_expiration(request->signed_trx);
				validate_tapos(request->signed_trx);
				validate_referenced_accounts(request->signed_trx);
				validate_uniqueness(request->signed_trx);

				return true;
			} FC_CAPTURE_AND_RETHROW((request->signed_trx))

			return false;
		}

		bool chain_xmax::should_check_for_duplicate_transactions() const
		{ 
			return !(_context->skip_flags & Config::skip_transaction_dupe_check);
		}
		bool chain_xmax::should_check_tapos() const 
		{ 
			return !(_context->skip_flags & Config::skip_tapos_check);
		}

		void chain_xmax::check_transaction_authorization(const signed_transaction& trx, bool allow_unused_signatures /*= false*/) const
		{
			//TODO
		}

		void chain_xmax::block_summary(const signed_block& next_block)
		{
			auto sid = next_block.block_num() & 0xffff;
			_context->block_db.modify(_context->block_db.get<block_summary_object, by_id>(sid), [&](block_summary_object& p) {
				p.block_id = next_block.id();
			});
		}

		void chain_xmax::update_or_create_builders(const builder_rule& builders)
		{
			for (const builder_info& info : builders.builders) 
			{
				if (const builder_object* obj = find_builder_object(info.builder_name))
				{
					if (obj->signing_key != info.block_signing_key)
					{
						_context->block_db.modify(*obj, [&](builder_object& newobj)
						{
							newobj.signing_key = info.block_signing_key;
						});
					}
				}
				else
				{
					_context->block_db.create<builder_object>([&](auto& pro) {
						pro.owner = info.builder_name;
						pro.signing_key = info.block_signing_key;
					});
				}
			}
		}

		void chain_xmax::parse_transaction(signed_transaction& result, const fc::variant& v) const
		{
			const variant_object& vo = v.get_object();
#define GET_FIELD( VO, FIELD, RESULT ) if( VO.contains(#FIELD) ) fc::from_variant( VO[#FIELD], RESULT.FIELD )

			GET_FIELD(vo, ref_block_num, result);
			GET_FIELD(vo, ref_block_prefix, result);
			GET_FIELD(vo, expiration, result);
			GET_FIELD(vo, scope, result);
			GET_FIELD(vo, signatures, result);

			if (vo.contains("messages")) {
				const vector<variant>& msgs = vo["messages"].get_array();
				result.messages.resize(msgs.size());
				for (uint32_t i = 0; i < msgs.size(); ++i) {
					const auto& vo = msgs[i].get_object();
					GET_FIELD(vo, code, result.messages[i]);
					GET_FIELD(vo, type, result.messages[i]);
					GET_FIELD(vo, authorization, result.messages[i]);

					if (vo.contains("data")) {
						const auto& data = vo["data"];
						if (data.is_string()) {
							GET_FIELD(vo, data, result.messages[i]);
						}
						else if (data.is_object()) {
							result.messages[i].data = message_to_binary(result.messages[i].code, result.messages[i].type, data);
						}
					}
				}
			}
#undef GET_FIELD

		}

		transaction_package_ptr chain_xmax::transaction_from_variant(const fc::variant& v) const
		{
			signed_transaction result;
			parse_transaction(result, v);
			return std::make_shared<transaction_package>(result);
#undef GET_FIELD
		}
		fc::variant       chain_xmax::transaction_to_variant(const transaction_response& response) const
		{
			fc::variant vo;
			fc::to_variant(response, vo);
			return vo;
		}
		fc::variant       chain_xmax::transaction_events_to_variant(const transaction_response& trx)const
		{

			vector<string> outstrings;
			for (const auto& it: trx.message_responses)
			{
				outstrings.push_back(it.out_string);
			}

			vector<fc::mutable_variant_object> outobjs(outstrings.size());
			vector<fc::variant> outs(outstrings.size());
			 
			for (int i = 0; i < outstrings.size(); ++i)
			{
				outobjs[i]("event", outstrings[i]);

				outs[i] = outobjs[i];
			}
			return outs;
		}


		void chain_xmax::require_account(const account_name& name) const
		{
			auto account = _context->block_db.find<account_object, by_name>(name);
			FC_ASSERT(account != nullptr, "Account not found: ${name}", ("name", name));
		}


		vector<signed_block> chain_xmax::get_syncblock_from_lastnum(const uint32_t& lastnum)
		{
			vector<signed_block> blockList;
			uint32_t currNum = _context->block_head->block_num;
			blockList.reserve(currNum - lastnum + 1);
			while (currNum)
			{	
	
				if (lastnum < currNum)
				{
					signed_block_ptr pSb = block_from_num(currNum);
					blockList.insert(blockList.begin(), *pSb);
					currNum--;
				}
				else
				{
					break;
				}
			}

			FC_ASSERT(blockList.size() > 0);

			return blockList;
			
		}


		chain_init::~chain_init() {}

} }
