/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <blockchain_types.hpp>
#include <block.hpp>
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

#include <transaction.hpp>
#include <objects/transaction_object.hpp>
#include <objects/generated_transaction_object.hpp>
#include <objects/block_summary_object.hpp>
#include <objects/account_object.hpp>
#include <objects/vote_objects.hpp>
#include <objects/resource_token_object.hpp>
#include <objects/xmx_token_object.hpp>

#include <vm_xmax.hpp>

#include <abi_serializer.hpp>

namespace Xmaxplatform { namespace Chain {

        void chain_xmax::setup_data_indexes() {
            _data.add_index<account_index>();

            _data.add_index<key_value_index>();
            _data.add_index<keystr_value_index>();
            _data.add_index<key128x128_value_index>();
            _data.add_index<key64x64x64_value_index>();

			_data.add_index<transaction_multi_index>();
			_data.add_index<generated_transaction_multi_index>();
			_data.add_index<block_summary_multi_index>();

            _data.add_index<static_config_multi_index>();
            _data.add_index<dynamic_states_multi_index>();
            _data.add_index<xmx_token_multi_index>();

			_data.add_index<voter_info_index>();
			_data.add_index<builder_info_index>();
			_data.add_index<resource_token_multi_index>();

        }

        void chain_xmax::initialize_chain(chain_init& initer)
        { try {
                if (!_data.find<static_config_object>()) {
                    _data.with_write_lock([this,&initer] {

						const fc::time_point init_point = initer.get_chain_init_time();;
						const chain_timestamp init_stamp = chain_timestamp::from(init_point);
						const chain_timestamp pre_stamp = init_stamp - chain_timestamp::create(1);

						xmax_builder_infos list;
						list.push_back(builder_info(Config::xmax_contract_name, Config::xmax_builder_key));

                        // Create global properties
                        _data.create<static_config_object>([&](static_config_object &p) {
                            p.setup = initer.get_blockchain_setup();
                            p.current_builders.set_builders(list, 0);
							p.new_builders.set_builders(p.current_builders.builders, 1);
                        });

                        _data.create<dynamic_states_object>([&](dynamic_states_object &p) {
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
							_data.create<block_summary_object>([&](block_summary_object&) {});


                        //signed_block block{};
                        //block.builder = Config::xmax_contract_name;
                        //block.threads.emplace_back();
                        //block.threads[0].emplace_back();

                        auto messages = initer.prepare_data(*this, _data);
                        std::for_each(messages.begin(), messages.end(), [&](const message_xmax& m) {
                            message_output output;
                            processed_transaction trx; /// dummy transaction required for scope validation
                            std::sort(trx.scope.begin(), trx.scope.end() );
                            with_skip_flags(0,[&](){
                                process_message(trx,m.code,m,output);
                            });

                            trx.messages.push_back(m);
                        });
                    });
                }

            } FC_CAPTURE_AND_RETHROW()

        }

        chain_xmax::chain_xmax(database& database,chain_init& init) : _data(database), _pending_txn_depth_limit(1000){

            setup_data_indexes();
            init.register_handlers(*this, _data);
                    with_applying_block([&] {
                        initialize_chain(init);
                    });

        }

        chain_xmax::~chain_xmax() {

            _data.flush();
        }

        const static_config_object& chain_xmax::get_static_config()const {
            return _data.get<static_config_object>();
        }

        const dynamic_states_object& chain_xmax::get_dynamic_states() const {
            return _data.get<dynamic_states_object>();
        }

        time chain_xmax::head_block_time() const {
            return get_dynamic_states().state_time;
        }

		uint32_t chain_xmax::head_block_num() const
		{
			return get_dynamic_states().head_block_number;
		}

		Xmaxplatform::Chain::xmax_type_block_id chain_xmax::head_block_id() const
		{
			return get_dynamic_states().head_block_id;
		}

		account_name chain_xmax::get_block_builder(uint32_t delta_slot) const
		{
			const dynamic_states_object& states = get_dynamic_states();
			const static_config_object& config = get_static_config();

			uint32_t slot = states.round_slot + delta_slot;

			if (slot < Config::blocks_per_round)
			{
				uint32_t index = slot % config.current_builders.number();
				return config.current_builders.builders[index].builder_name;
			}

			if (config.next_builders.is_empty()) // no next list.
			{
				uint32_t index = slot % config.current_builders.number();
				return config.current_builders.builders[index].builder_name;
			}
			// get builder in next list.
			uint32_t deltaslot = slot - config.current_builders.number();

			uint32_t bias = slot % config.next_builders.number();

			return config.next_builders.builders[bias].builder_name;
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

		vector<char> chain_xmax::message_to_binary(name code, name type, const fc::variant& obj)const
		{
			try {
				const auto& code_account = _data.get<account_object, by_name>(code);
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
			const auto& code_account = _data.get<account_object, by_name>(code);
			Xmaxplatform::Basetypes::abi abi;
			if (Basetypes::abi_serializer::to_abi(code_account.abi, abi)) {
				Basetypes::abi_serializer abis(abi);
				return abis.binary_to_variant(abis.get_action_type(type), bin);
			}
			return fc::variant();
		}

		Xmaxplatform::Chain::processed_transaction chain_xmax::push_transaction(const signed_transaction& trx, uint32_t skip /*= skip_nothing*/)
		{
			try {
				return with_skip_flags(skip | pushed_transaction, [&]() {
					return _data.with_write_lock([&]() {
						return _push_transaction(trx);
					});
				});
			} FC_CAPTURE_AND_RETHROW((trx))
		}

		Xmaxplatform::Chain::processed_transaction chain_xmax::_push_transaction(const signed_transaction& trx)
		{
			if (!_pending_tx_session.valid())
				_pending_tx_session = _data.start_undo_session(true);

			FC_ASSERT(_pending_transactions.size() < 1000, "too many pending transactions, try again later");

			auto temp_session = _data.start_undo_session(true);
			validate_referenced_accounts(trx);
			check_transaction_authorization(trx);
			auto pt = apply_transaction(trx);
			_pending_transactions.push_back(trx);

			temp_session.squash();

			on_pending_transaction(trx); 

			return pt;
		}

		signed_block chain_xmax::generate_block(
                chain_timestamp when,
                const account_name& builder
        )
        { try {
                _data.start_undo_session(true);
                auto b = _data.with_write_lock( [&](){
                    return _generate_block( when, builder );
                });
                push_block(b);
                return b;
            } FC_CAPTURE_AND_RETHROW( (when) ) }

        signed_block chain_xmax::_generate_block(
                chain_timestamp when,
                const account_name& builder
        )
        {
            try {
				FC_ASSERT(head_block_time() < when.time_point(), "block must be generated at a timestamp after the head block time");

				const dynamic_states_object& dy_state = get_dynamic_states();

				time_point start = fc::time_point::now();

                signed_block pending_block;

                uint32_t pending_block_size = fc::raw::pack_size( pending_block );

                pending_block.previous = dy_state.head_block_id;
                pending_block.timestamp = when;
                pending_block.transaction_merkle_root = pending_block.calculate_merkle_root();



                pending_block.builder = builder;

				if (builders_elected == dy_state.builders_elect_state)
				{
					pending_block.next_builders = get_static_config().new_builders;
				}

                const auto end = fc::time_point::now();
                const auto gen_time = end - start;
                if( gen_time > fc::milliseconds(10) ) {
                    ilog("generation took ${x} ms", ("x", gen_time.count() / 1000));
                    FC_ASSERT(gen_time < fc::milliseconds(250), "block took too long to build");
                }



                //pending_block.sign( block_signing_private_key );

                return pending_block;
            } FC_CAPTURE_AND_RETHROW( (builder) ) }

        bool chain_xmax::push_block(const signed_block& new_block)
        { try {
                return _data.with_write_lock( [&]() {
                    return _push_block(new_block);
                        });

            } FC_CAPTURE_AND_RETHROW((new_block)) }

        bool chain_xmax::_push_block(const signed_block& new_block)
        { try {

                try {
                    auto session = _data.start_undo_session(true);
                    auto exec_start = std::chrono::high_resolution_clock::now();
                    apply_block(new_block);
                    auto exec_stop = std::chrono::high_resolution_clock::now();
                    auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_stop - exec_start);
                    ilog( "${builder} generate block #${num}  at ${time}, exectime_ms=${extm}",
                          ("builder", new_block.builder)
                                  ("time", new_block.timestamp)
                                  ("num", new_block.block_num())
                                  ("extm", exec_ms.count())
                    );

                    session.push();
                } catch ( const fc::exception& e ) {
                    elog("Failed to push new block:\n${e}", ("e", e.to_detail_string()));
                    throw;
                }

                return false;
            } FC_CAPTURE_AND_RETHROW((new_block)) }

        void chain_xmax::apply_block(const signed_block& next_block)
        {

            with_applying_block([&] {
                    _apply_block(next_block);
            });
        }

        void chain_xmax::_apply_block(const signed_block& next_block)
        { try {

				FC_ASSERT(next_block.transaction_merkle_root == next_block.calculate_merkle_root(), "action merkle root does not match");

				const dynamic_states_object& dy_state = get_dynamic_states();
				if (builders_elected == dy_state.builders_elect_state)
				{
					const static_config_object& config = get_static_config();
					FC_ASSERT(next_block.next_builders.valid() && (*next_block.next_builders == config.new_builders), "error builder list.");
				}

                _finalize_block(next_block);

				create_block_summary(next_block);

            } FC_CAPTURE_AND_RETHROW( (next_block.block_num()) )  }


		void chain_xmax::_finalize_block(const signed_block& b) 
		{		
			const dynamic_states_object& dy_state = get_dynamic_states();

			chain_timestamp current_block_time = b.timestamp;

			elect_state builder_elect_state = dy_state.builders_elect_state;

			chain_timestamp round_begin_time = dy_state.round_begin_time;

			uint32_t delta_slot = get_delta_slot_at_time(current_block_time);

			uint32_t total_slot = dy_state.total_slot + delta_slot;

			uint32_t current_round_slot = dy_state.round_slot + delta_slot;

			// missed_block
			//uint32_t missed_blocks = delta_slot - 1;


			if (b.next_builders.valid()) // store next builder list.
			{
				const static_config_object& static_config = get_static_config();
				_data.modify(static_config, [&](static_config_object& obj) {
					obj.next_builders = *b.next_builders;
				});
				builder_elect_state = builders_confirmed;
			}
			if (elect_new_builders == builder_elect_state) // create new builder list.
			{
				xmax_builder_infos new_builders = Native_contract::xmax_voting::next_round(_data);
				
				const static_config_object& static_config = get_static_config();

				uint32_t new_version = static_config.current_builders.version + 1;

				_data.modify(static_config, [&](static_config_object& obj) {
					obj.new_builders.set_builders(new_builders, new_version);
				});		
				builder_elect_state = builders_elected;

				std::stringstream namestream;
				for (const builder_info& it : new_builders)
				{
					namestream << it.builder_name.to_string() << ",";
				}
				fc::mutable_variant_object capcture;
				capcture.set("builders", namestream.str());

				ilog("next round: ${builders}", (capcture));
			}

			if (current_round_slot >= Config::blocks_per_round)
			{
				const static_config_object& static_config = get_static_config();
				if (!static_config.next_builders.is_empty())
				{
					builder_rule current_builders = static_config.next_builders;
					_data.modify(static_config, [&](static_config_object& obj) {
						obj.current_builders = current_builders;
						obj.new_builders.reset();
						obj.next_builders.reset();
					});
					builder_elect_state = elect_new_builders;

					// new builders state

					current_round_slot = current_round_slot % Config::blocks_per_round;
					chain_timestamp delta_time = chain_timestamp::create(current_round_slot);
					round_begin_time = current_block_time - delta_time;

				}
			}

			//else if (builders_confirmed == builder_elect_state)
			{
				//const static_config_object& static_config = get_static_config();

				//builder_rule current_builders = static_config.next_builders;
				//_data.modify(static_config, [&](static_config_object& obj) {
				//	obj.current_builders = current_builders;
				//	obj.new_builders.reset();
				//	obj.next_builders.reset();
				//});
				//builder_elect_state = elect_new_builders;
			}

            // update dynamic states
            _data.modify( dy_state, [&]( dynamic_states_object& dgp ){
                dgp.head_block_number = b.block_num();
                dgp.head_block_id = b.id();
                dgp.state_time = b.timestamp.time_point();
                dgp.block_builder = b.builder;
				dgp.round_begin_time = round_begin_time;
				dgp.total_slot = total_slot;
				dgp.round_slot = current_round_slot;
				dgp.builders_elect_state = builder_elect_state;
            });




        }


        void chain_xmax::set_message_handler( const account_name& contract, const account_name& scope, const action_name& action, msg_handler v ) {
            message_handlers[contract][std::make_pair(scope,action)] = v;
        }

        void chain_xmax::process_message(const transaction& trx, account_name code,
                                         const message_xmax& message, message_output& output,
                                         message_context_xmax* parent_context, int depth,
                                         const fc::time_point& start_time ) {

            auto us_duration = (fc::time_point::now() - start_time).count();

            message_context_xmax xmax_ctx(*this, _data, trx, message, code);
            apply_message(xmax_ctx);

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
                auto contract_handlers_itr = message_handlers.find(context.code);
                if (contract_handlers_itr != message_handlers.end()) {
                    auto message_handler_itr = contract_handlers_itr->second.find({m.code, m.type});
                    if (message_handler_itr != contract_handlers_itr->second.end()) {
                        message_handler_itr->second(context);
                        return;
                    }
                }
                const auto& recipient = _data.get<account_object,by_name>(context.code);
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

			auto transaction = _data.find<transaction_object, by_trx_id>(trx.id());
			XMAX_ASSERT(transaction == nullptr, tx_duplicate, "Transaction is not unique");
		}

		void chain_xmax::validate_uniqueness(const generated_transaction& trx)const {
			if (!should_check_for_duplicate_transactions()) return;
		}

		void chain_xmax::record_transaction(const Chain::signed_transaction& trx) {
			//Insert transaction into unique transactions database.
			_data.create<transaction_object>([&](transaction_object& transaction) {
				transaction.trx_id = trx.id(); /// TODO: consider caching ID
				transaction.expiration = trx.expiration;
			});
		}

		void chain_xmax::record_transaction(const generated_transaction& trx) {
			_data.modify(_data.get<generated_transaction_object, generated_transaction_object::by_trx_id>(trx.id), [&](generated_transaction_object& transaction) {
				transaction.status = generated_transaction_object::PROCESSED;
			});
		}




		void chain_xmax::validate_tapos(const transaction& trx)const {
			if (!should_check_tapos()) return;

			const auto& tapos_block_summary = _data.get<block_summary_object>((uint16_t)trx.ref_block_num);

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
			//TODO
			return optional<signed_block>();
		}
		optional<signed_block>      chain_xmax::get_block_from_num(uint32_t num)const
		{
			//TODO
			return optional<signed_block>();
		}

		void chain_xmax::create_block_summary(const signed_block& next_block)
		{
			auto sid = next_block.block_num() & 0xffff;
			_data.modify(_data.get<block_summary_object, by_id>(sid), [&](block_summary_object& p) {
				p.block_id = next_block.id();
			});
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
			auto account = _data.find<account_object, by_name>(name);
			FC_ASSERT(account != nullptr, "Account not found: ${name}", ("name", name));
		}

		chain_init::~chain_init() {}

} }
