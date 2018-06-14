/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */


#include <blockchain_types.hpp>
#include <block.hpp>
#include <blockchain_exceptions.hpp>
#include <basechain.hpp>
#include <forkchain.hpp>
#include <chain_init.hpp>
#include <chain_xmax.hpp>
#include <xmax_voting.hpp>

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

#include <objects/transaction_object.hpp>
#include <objects/generated_transaction_object.hpp>
#include <objects/block_summary_object.hpp>
#include <objects/account_object.hpp>
#include <objects/vote_objects.hpp>
#include <objects/resource_token_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/builder_object.hpp>
#include <objects/block_object.hpp>
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
		chain_stream						chain_log;
		chain_xmax::xmax_config				config;
		database							block_db;
		forkdatabase						fork_db;
		uint32_t							last_irreversible_block_num = 0;

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

		}
		~chain_context()
		{
			building_block.reset();
			block_db.flush();
		}

		xmax_type_merkle_root calculate_merkle_root() const
		{
			// empty now. test...
#pragma message("Unrealized functions, realize in the future.") 
			return xmax_type_merkle_root();
		}

		//template<typename Function>
		//auto with_applying_block(Function&& f) -> decltype((*((Function*)nullptr))()) {
		//	auto on_exit = fc::make_scoped_exit([this]() {
		//		currently_applying_block = false;
		//	});
		//	currently_applying_block = true;
		//	return f();
		//}

		template<typename Function>
		auto with_skip_flags(uint64_t flags, Function&& f) -> decltype((*((Function*)nullptr))())
		{
			auto old_flags = skip_flags;
			auto on_exit = fc::make_scoped_exit([&]() {skip_flags = old_flags; });
			skip_flags = flags;
			return f();
		}

	};


        void chain_xmax::setup_data_indexes() {
            _context->block_db.add_index<account_index>();

            _context->block_db.add_index<key_value_index>();
            _context->block_db.add_index<keystr_value_index>();
            _context->block_db.add_index<key128x128_value_index>();
            _context->block_db.add_index<key64x64x64_value_index>();

			_context->block_db.add_index<transaction_multi_index>();
			_context->block_db.add_index<generated_transaction_multi_index>();
			_context->block_db.add_index<block_summary_multi_index>();
			_context->block_db.add_index<block_multi_index>();

            _context->block_db.add_index<static_config_multi_index>();
            _context->block_db.add_index<dynamic_states_multi_index>();
            _context->block_db.add_index<xmx_token_multi_index>();

			_context->block_db.add_index<voter_info_index>();
			_context->block_db.add_index<builder_info_index>();
			_context->block_db.add_index<builder_multi_index>();
			_context->block_db.add_index<resource_token_multi_index>();

        }

        void chain_xmax::initialize_chain(chain_init& initer)
        { try {
                if (!_context->block_db.find<static_config_object>()) {
                    _context->block_db.with_write_lock([this,&initer] {

						const fc::time_point init_point = initer.get_chain_init_time();;
						const chain_timestamp init_stamp = chain_timestamp::from(init_point);
						const chain_timestamp pre_stamp = init_stamp - chain_timestamp::create(1);

						xmax_builder_infos list;
						list.push_back(builder_info(Config::xmax_contract_name, Config::xmax_build_public_key));

                        // Create global properties
                        _context->block_db.create<static_config_object>([&](static_config_object &p) {
                            p.setup = initer.get_blockchain_setup();
                            p.current_builders.set_builders(list, 0);
							p.next_builders.set_builders(p.current_builders.builders, 1);
                        });

                        _context->block_db.create<dynamic_states_object>([&](dynamic_states_object &p) {
							p.head_block_number = 0;
							p.head_block_id = xmax_type_block_id();
                            p.state_time = pre_stamp.time_point();
							p.total_slot = 0;
                            p.block_builder = empty_name;
							p.round_begin_time = chain_timestamp::zero_timestamp;
							p.round_slot = Config::blocks_per_round;
							p.builders_elect_state = elect_state::elect_new_builders;
                        });

						for (int i = 0; i < 0x10000; i++)
							_context->block_db.create<block_summary_object>([&](block_summary_object&) {});


                        //signed_block block{};
                        //block.builder = Config::xmax_contract_name;
                        //block.threads.emplace_back();
                        //block.threads[0].emplace_back();

                        auto messages = initer.prepare_data(*this, _context->block_db);
                        std::for_each(messages.begin(), messages.end(), [&](const message_xmax& m) {
                            message_output output;
                            processed_transaction trx; /// dummy transaction required for scope validation
                            std::sort(trx.scope.begin(), trx.scope.end() );
							_context->with_skip_flags(0,[&](){
                                process_message(trx,m.code,m,output);
                            });

                            trx.messages.push_back(m);
                        });
                    });
                }

            } FC_CAPTURE_AND_RETHROW()

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

            _context->block_db.flush();
        }

        const static_config_object& chain_xmax::get_static_config()const {
            return _context->block_db.get<static_config_object>();
        }

        const dynamic_states_object& chain_xmax::get_dynamic_states() const {
            return _context->block_db.get<dynamic_states_object>();
        }

        time chain_xmax::head_block_time() const {
            return get_dynamic_states().state_time;
        }

		uint32_t chain_xmax::head_block_num() const
		{
			return get_dynamic_states().head_block_number;
		}

		uint32_t chain_xmax::last_irreversible_block_num() const
		{
			return _context->last_irreversible_block_num;
		}

		Xmaxplatform::Chain::xmax_type_block_id chain_xmax::head_block_id() const
		{
			return get_dynamic_states().head_block_id;
		}

		const builder_info& _get_builder(const static_config_object& config, uint32_t index)
		{
			if (index < Config::blocks_per_round)
			{
				uint32_t bias = index % config.current_builders.number();
				return config.current_builders.builders[bias];
			}

			if (config.next_builders.is_empty()) // no next list.
			{
				uint32_t bias = index % config.current_builders.number();
				return config.current_builders.builders[bias];
			}
			// get builder in next list.
			uint32_t deltaslot = index - config.current_builders.number();

			uint32_t bias = index % config.next_builders.number();

			return config.next_builders.builders[bias];
		}

		const builder_info& chain_xmax::get_block_builder(uint32_t delta_slot) const
		{
			asset(delta_slot >= 0);
			const dynamic_states_object& states = get_dynamic_states();
			const static_config_object& config = get_static_config();

			uint32_t slot = states.round_slot + delta_slot;

			return _get_builder(config, slot);
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
			chain_timestamp first_slot_time = get_delta_slot_time(1);
			if (when < first_slot_time)
				return 0;

			chain_timestamp sub = when - first_slot_time;
			return sub.get_stamp() + 1;
		}
        chain_timestamp chain_xmax::get_delta_slot_time(uint32_t delta_slot) const
        {
            if (0 == delta_slot)
            {
				return chain_timestamp::zero_timestamp;
            }

			chain_timestamp head_block_abs_slot = chain_timestamp::from(head_block_time());
			head_block_abs_slot += chain_timestamp::create(delta_slot);
			return head_block_abs_slot;

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


		//--------------------------------------------------
		Xmaxplatform::Chain::processed_transaction chain_xmax::transaction_from_variant(const fc::variant& v) const {
			const variant_object& vo = v.get_object();
#define GET_FIELD( VO, FIELD, RESULT ) \
   if( VO.contains(#FIELD) ) fc::from_variant( VO[#FIELD], RESULT.FIELD )

			processed_transaction result;
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
			if (vo.contains("output")) {
				const vector<variant>& outputs = vo["output"].get_array();
			}
			return result;
#undef GET_FIELD
		}

		//--------------------------------------------------
		fc::variant chain_xmax::transaction_to_variant(const processed_transaction& trx) const
		{
#define SET_FIELD( MVO, OBJ, FIELD ) MVO(#FIELD, OBJ.FIELD)

			fc::mutable_variant_object trx_mvo;
			SET_FIELD(trx_mvo, trx, ref_block_num);
			SET_FIELD(trx_mvo, trx, ref_block_prefix);
			SET_FIELD(trx_mvo, trx, expiration);
			SET_FIELD(trx_mvo, trx, scope);
			SET_FIELD(trx_mvo, trx, signatures);

			vector<fc::mutable_variant_object> msgs(trx.messages.size());
			vector<fc::variant> msgsv(msgs.size());


			for (uint32_t i = 0; i < trx.messages.size(); ++i) {
				auto& msg_mvo = msgs[i];
				auto& msg = trx.messages[i];
				SET_FIELD(msg_mvo, msg, code);
				SET_FIELD(msg_mvo, msg, type);
				SET_FIELD(msg_mvo, msg, authorization);

				const auto& code_account = _context->block_db.get<account_object, by_name>(msg.code);
				if (!Basetypes::abi_serializer::is_empty_abi(code_account.abi)) {
					try {
						msg_mvo("data", message_from_binary(msg.code, msg.type, msg.data));
						msg_mvo("hex_data", msg.data);
					}
					catch (...) {
						SET_FIELD(msg_mvo, msg, data);
					}
				}
				else {
					SET_FIELD(msg_mvo, msg, data);
				}

				msgsv[i] = std::move(msgs[i]);
			}
			trx_mvo("messages", std::move(msgsv));
			trx_mvo("output", fc::variant(trx.output));

			return fc::variant(std::move(trx_mvo));
#undef SET_FIELD
		}


		//--------------------------------------------------
		fc::variant chain_xmax::transaction_events_to_variant(const processed_transaction& trx) const {
#define SET_FIELD(MVO, OBJ, FIELD) MVO(#FIELD, OBJ.FIELD)

			fc::mutable_variant_object events_mvo;

			vector<event_output> evtv;

			for (auto& message_output : trx.output)
			{
				for (auto& event_output : message_output.events)
				{
					evtv.push_back(event_output);
				}
			}

			vector<fc::mutable_variant_object> evtomv(evtv.size());
			vector<fc::variant> evtov(evtomv.size());
			for (int i = 0; i < evtv.size(); ++i)
			{
				auto& evt = evtv[i];
				auto& evto = evtomv[i];
				SET_FIELD(evto, evt, name);
				SET_FIELD(evto, evt, code);
				SET_FIELD(evto, evt, type);
				//SET_FIELD( evto, evt, data );
				evto("data", event_from_binary(evt.code, evt.event_type_name, evt.data));

				evtov[i] = std::move(evtomv[i]);
			}

			//events_mvo( "events", fc::variant( evtov ) );

			return evtov;
#undef SET_FIELD
		}

		Xmaxplatform::Chain::processed_transaction chain_xmax::push_transaction(const signed_transaction& trx, uint32_t skip /*= skip_nothing*/)
		{
			try {
				return _context->with_skip_flags(skip | pushed_transaction, [&]() {
					transaction_request_ptr request = std::make_shared<transaction_request>(trx);
					return _push_transaction(request);
				});
			} FC_CAPTURE_AND_RETHROW((trx))
		}

		Xmaxplatform::Chain::processed_transaction chain_xmax::_push_transaction(transaction_request_ptr request)
		{

			FC_ASSERT(_context->pending_transactions.size() < 1000, "too many pending transactions, try again later");



			auto temp_session = _context->block_db.start_undo_session(true);
			validate_referenced_accounts(request->signed_trx);
			check_transaction_authorization(request->signed_trx);
			auto pt = apply_transaction(request->signed_trx);
			_context->pending_transactions.push_back(request);

			temp_session.squash();

			on_pending_transaction(request->signed_trx);

			return pt;
		}
		

		//--------------------------------------------------
		Xmaxplatform::Chain::flat_set<Xmaxplatform::Chain::public_key_type> chain_xmax::get_required_keys(const signed_transaction& transaction, const flat_set<public_key_type>& candidateKeys) const
		{
			// Todo: Authentication is to be supported later. Return keys directly now.
			return flat_set<Xmaxplatform::Chain::public_key_type>(candidateKeys);			
		}

		void chain_xmax::build_block(
                chain_timestamp when,
                const account_name& builder,
				const private_key_type& sign_private_key
        ) { 

			_abort_build();

			auto exec_start = std::chrono::high_resolution_clock::now();

			_start_build(when, builder);

			_build_block(when, builder, sign_private_key);

			auto exec_stop = std::chrono::high_resolution_clock::now();
			auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_stop - exec_start);

			const auto& new_block = _context->building_block->pack->block;

			ilog("${builder} generate block #${num}  at ${time}, exectime_ms=${extm}",
				("builder", new_block->builder)
				("time", new_block->timestamp)
				("num", new_block->block_num())
				("extm", exec_ms.count())
			);

			// temp process. commit impl.
			_commit_block();

			// tmp code irreversible immediately
			//_irreversible_block(_context->building_block->pack->block);
		}

		void chain_xmax::_abort_build()
		{
			if (_context->building_block)
			{
				_context->building_block.reset();
			}
		}

		void chain_xmax::_start_build(chain_timestamp when, const account_name& builder)
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

				// prepare data.
				const dynamic_states_object& dy_state = get_dynamic_states();

				time_point start = fc::time_point::now();

				signed_block_header& building_header = _context->building_block->pack->new_header;

				// build block.
				building_header.previous = dy_state.head_block_id;
				building_header.timestamp = when;
				building_header.builder = builder;

				pending_undo.cancel();

			} FC_CAPTURE_AND_RETHROW((builder))
		}

        void chain_xmax::_build_block(
                chain_timestamp when,
                const account_name& builder,
				const private_key_type& sign_private_key
        ) {
			FC_ASSERT(_context->building_block);
            try {

				signed_block_header& building_header = _context->building_block->pack->new_header;

				const dynamic_states_object& dy_state = get_dynamic_states();

				// create new builder list. before sign.
				if (elect_new_builders == dy_state.builders_elect_state)
				{
					xmax_builder_infos new_builders = Native_contract::xmax_voting::next_round(_context->block_db);
					
					const static_config_object& static_config = get_static_config();
					uint32_t new_version = static_config.current_builders.version + 1;
					builder_rule newrule;
					newrule.set_builders(new_builders, new_version);
					building_header.next_builders = newrule;
					std::stringstream namestream;
					for (const builder_info& it : new_builders)
					{
						namestream << it.builder_name.to_string() << ",";
					}
					fc::mutable_variant_object capcture;
					capcture.set("builders", namestream.str());

					ilog("next round: ${builders}", (capcture));
				}

				// merkle
				building_header.transaction_merkle_root = _context->calculate_merkle_root();

				building_header.sign(sign_private_key);

				_context->building_block->pack->block_id = building_header.id();

				_context->building_block->pack->block = std::make_shared<signed_block>();

				signed_block_header* block_ptr = _context->building_block->pack->block.get();

				(*block_ptr) = building_header;

				_update_block_state(*_context->building_block->pack->block);

            } FC_CAPTURE_AND_RETHROW( (builder) ) 
		}


		void chain_xmax::_commit_block() {

			FC_ASSERT(_context->building_block);
			const auto& new_block = _context->building_block->pack->block;

			try {

				_context->fork_db.add_block(_context->building_block->pack);

				block_summary(*new_block);

				_context->building_block->push_block();

			} FC_CAPTURE_AND_RETHROW((new_block->block_num()))
		
		}

        void chain_xmax::apply_block(const signed_block& next_block) {

			_abort_build();
			_apply_block(next_block);

        }

        void chain_xmax::_apply_block(const signed_block& next_block) { 
			try {

				FC_ASSERT(next_block.transaction_merkle_root == next_block.calculate_merkle_root(), "action merkle root does not match");

				const dynamic_states_object& dy_state = get_dynamic_states();
				if (elect_state::elect_new_builders == dy_state.builders_elect_state)
				{
					const static_config_object& config = get_static_config();
					FC_ASSERT(next_block.next_builders.valid(), "empty builder list.");
				}
				

                _update_block_state(next_block);

				block_summary(next_block);

            } FC_CAPTURE_AND_RETHROW( (next_block.block_num()) ) 
		}

		void chain_xmax::_update_block_state(const signed_block& last_block)
		{		
			
			const dynamic_states_object& dy_state = get_dynamic_states();

			chain_timestamp current_block_time = last_block.timestamp;

			elect_state builder_elect_state = dy_state.builders_elect_state;

			chain_timestamp round_begin_time = dy_state.round_begin_time;

			uint32_t delta_slot = get_delta_slot_at_time(current_block_time);

			uint32_t total_slot = dy_state.total_slot + delta_slot;

			uint32_t current_round_slot = dy_state.round_slot + delta_slot;

			// missed_block
			uint32_t missed_blocks = delta_slot - 1;
			// info of miss builders.
			if (missed_blocks > 0)
			{
				uint32_t pre_slot = dy_state.round_slot + 1;
				for (int i = 0; i < missed_blocks; ++i)
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
			if (last_block.next_builders.valid()) 
			{
				update_or_create_builders(*last_block.next_builders);

				const static_config_object& static_config = get_static_config();
				_context->block_db.modify(static_config, [&](static_config_object& obj) {
					obj.next_builders = *last_block.next_builders;
				});
				builder_elect_state = builders_confirmed;
			}

			// change to next round.
			if (current_round_slot >= Config::blocks_per_round)
			{
				const static_config_object& static_config = get_static_config();
				if (!static_config.next_builders.is_empty())
				{
					builder_rule next_round = static_config.next_builders;
					_context->block_db.modify(static_config, [&](static_config_object& obj) {
						obj.current_builders = next_round;
						obj.next_builders.reset();
					});
					builder_elect_state = elect_new_builders;

					//// new builders state

					//current_round_slot = current_round_slot % Config::blocks_per_round;
					//chain_timestamp delta_time = chain_timestamp::create(current_round_slot);
					//round_begin_time = current_block_time - delta_time;

				}
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
				dgp.round_begin_time = round_begin_time;
				dgp.total_slot = total_slot;
				dgp.round_slot = current_round_slot;
				dgp.builders_elect_state = builder_elect_state;
            });

			// update block data
			_context->block_db.create<block_object>([&](block_object& blk) {
				blk.blk_id = last_block.id();
				blk.block = last_block;
			});

			on_finalize_block(last_block);
				
        }

		void chain_xmax::_irreversible_block(const block_pack_ptr& pack)
		{
			uint32_t block_num = pack->block->block_num();

			_context->last_irreversible_block_num = block_num;

			_context->block_db.commit(block_num);

			_context->chain_log.append_block(pack->block);

		}

        void chain_xmax::set_message_handler( const account_name& contract, const account_name& scope, const action_name& action, msg_handler v ) {
			_context->message_handlers[contract][std::make_pair(scope,action)] = v;
        }

        void chain_xmax::process_message(const transaction& trx, account_name code,
                                         const message_xmax& message, message_output& output,
                                         message_context_xmax* parent_context, int depth,
                                         const fc::time_point& start_time ) {

            auto us_duration = (fc::time_point::now() - start_time).count();

            message_context_xmax xmax_ctx(*this, _context->block_db, trx, message, code);
            apply_message(xmax_ctx);

			for (auto& event_output : xmax_ctx.events)
			{
				output.events.push_back(std::move(event_output));
			}

            output.notify.reserve( xmax_ctx.notified.size() );

            for( uint32_t i = 0; i < xmax_ctx.notified.size(); ++i ) {
                try {
                    auto notify_code = xmax_ctx.notified[i];
                    output.notify.push_back( {notify_code} );
                    process_message(trx, notify_code, message, output.notify.back().output, &xmax_ctx, depth + 1, start_time );
                } FC_CAPTURE_AND_RETHROW((xmax_ctx.notified[i]))
            }

            // combine inline messages and process
            if (xmax_ctx.inline_messages.size() > 0) {
                output.inline_trx = inline_transaction(trx);
                (*output.inline_trx).messages = std::move(xmax_ctx.inline_messages);
            }


        }


        void chain_xmax::apply_message(message_context_xmax& context)
        { try {
                /// context.code => the execution namespace
                /// message.code / message.type => Event
                const auto& m = context.msg;
                auto contract_handlers_itr = _context->message_handlers.find(context.code);
                if (contract_handlers_itr != _context->message_handlers.end()) {
                    auto message_handler_itr = contract_handlers_itr->second.find({m.code, m.type});
                    if (message_handler_itr != contract_handlers_itr->second.end()) {
                        message_handler_itr->second(context);
                        return;
                    }
                }
                const auto& recipient = _context->block_db.get<account_object,by_name>(context.code);
                if (recipient.code.size()) {
                    idump((context.code)(context.msg.type));
                    const uint32_t execution_time = 10000;//TODO
                    try {
                        vm_xmax::get().apply(context, execution_time, true );
                    } catch (const fc::exception &ex) {

                    }
                }
            } FC_CAPTURE_AND_RETHROW((context.msg)) }

		void chain_xmax::validate_uniqueness(const Chain::signed_transaction& trx)const {
			if (!should_check_for_duplicate_transactions()) return;

			auto transaction = _context->block_db.find<transaction_object, by_trx_id>(trx.id());
			XMAX_ASSERT(transaction == nullptr, tx_duplicate, "Transaction is not unique");
		}

		void chain_xmax::validate_uniqueness(const generated_transaction& trx)const {
			if (!should_check_for_duplicate_transactions()) return;
		}

		void chain_xmax::record_transaction(const Chain::signed_transaction& trx) {
			//Insert transaction into unique transactions database.
			_context->block_db.create<transaction_object>([&](transaction_object& transaction) {
				transaction.trx_id = trx.id(); /// TODO: consider caching ID
				transaction.expiration = trx.expiration;
			});
		}

		void chain_xmax::record_transaction(const generated_transaction& trx) {
			_context->block_db.modify(_context->block_db.get<generated_transaction_object, generated_transaction_object::by_trx_id>(trx.id), [&](generated_transaction_object& transaction) {
				transaction.status = generated_transaction_object::PROCESSED;
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
				fc::time_point_sec now = head_block_time();
				const blockchain_configuration& chain_configuration = get_static_config().setup;

// 				XMAX_ASSERT(trx.expiration <= now + int32_t(chain_configuration.max_trx_lifetime),
// 					transaction_exception, "Transaction expiration is too far in the future",
// 					("trx.expiration", trx.expiration)("now", now)
// 					("max_til_exp", chain_configuration.max_trx_lifetime));
// 				XMAX_ASSERT(now <= trx.expiration, transaction_exception, "Transaction is expired",
// 					("now", now)("trx.exp", trx.expiration));
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
		bool chain_xmax::should_check_for_duplicate_transactions() const
		{ 
			return !(_context->skip_flags & skip_transaction_dupe_check);
		}
		bool chain_xmax::should_check_tapos() const 
		{ 
			return !(_context->skip_flags & skip_tapos_check);
		}

		void chain_xmax::check_transaction_authorization(const signed_transaction& trx, bool allow_unused_signatures /*= false*/) const
		{
			//TODO
		}

		xmax_type_block_id              chain_xmax::get_blockid_from_num(uint32_t block_num)const
		{
			try {
				if (const auto& block = get_block_from_num(block_num))
					return block->id();

				FC_THROW_EXCEPTION(unknown_block_exception, "Could not find block");
			} FC_CAPTURE_AND_RETHROW((block_num))
		}

		optional<signed_block>			chain_xmax::get_block_from_id(const xmax_type_block_id& id)const
		{			

			const auto& block_obj = _context->block_db.get<block_object, by_blk_id>(id);
			return block_obj.block;
		}
		optional<signed_block>      chain_xmax::get_block_from_num(uint32_t num)const
		{
			const auto& block_obj = _context->block_db.get<block_object, by_blk_num>(num);
			return block_obj.block;
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

		template<typename T>
		typename T::processed chain_xmax::apply_transaction(const T& trx)
		{
			try {
				validate_transaction(trx);
				record_transaction(trx);
				return process_transaction(trx, 0, fc::time_point::now());

			} FC_CAPTURE_AND_RETHROW((trx))
		}

		template<typename T>
		typename T::processed chain_xmax::process_transaction(const T& trx, int depth, const fc::time_point& start_time)
		{
			try {
				const blockchain_configuration& chain_configuration = get_static_config().setup;
				XMAX_ASSERT((fc::time_point::now() - start_time).count() < chain_configuration.max_trx_runtime, checktime_exceeded,
					"Transaction exceeded maximum total transaction time of ${limit}ms", ("limit", chain_configuration.max_trx_runtime / 1000));

				XMAX_ASSERT(depth < chain_configuration.in_depth_limit, tx_resource_exhausted,
					"Transaction exceeded maximum inline recursion depth of ${limit}", ("limit", chain_configuration.in_depth_limit));

				typename T::processed ptrx(trx);
				ptrx.output.resize(trx.messages.size());

				for (uint32_t i = 0; i < trx.messages.size(); ++i) {
					auto& output = ptrx.output[i];
					//rate_limit_message(trx.messages[i]); no limit for now
					process_message(trx, trx.messages[i].code, trx.messages[i], output, nullptr, 0, start_time);
					if (output.inline_trx.valid()) {
						const transaction& trx = *output.inline_trx;
						output.inline_trx = process_transaction(pending_inline_transaction(trx), depth + 1, start_time);
					}
				}

				return ptrx;
			} FC_CAPTURE_AND_RETHROW((trx))
		}

		void chain_xmax::require_account(const account_name& name) const
		{
			auto account = _context->block_db.find<account_object, by_name>(name);
			FC_ASSERT(account != nullptr, "Account not found: ${name}", ("name", name));
		}

		chain_init::~chain_init() {}

} }
