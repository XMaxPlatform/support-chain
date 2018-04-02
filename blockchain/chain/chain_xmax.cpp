/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <blockchain_types.hpp>
#include <block.hpp>
#include <chain_xmax.hpp>


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

namespace Xmaxplatform { namespace Chain {


        void chain_xmax::setup_data_indexes() {
            _data.add_index<static_config_multi_index>();
            _data.add_index<dynamic_states_multi_index>();

        }

        void chain_xmax::initialize_chain()
        { try {
                if (!_data.find<static_config_object>()) {
                    _data.with_write_lock([&]() {

                        // Create global properties
                        _data.create<static_config_object>([&](static_config_object &p) {

                        });
                        _data.create<dynamic_states_object>([&](dynamic_states_object &p) {

                        });

                        signed_block block{};
                        block.builder = Config::xmax_contract_name;
                        block.cycles.emplace_back();
                        block.cycles[0].emplace_back();
                    });
                }

            } FC_CAPTURE_AND_RETHROW()

        }

        chain_xmax::chain_xmax(database& database) : _data(database){

            setup_data_indexes();
                    with_applying_block([&] {
                        initialize_chain();
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

        signed_block chain_xmax::generate_block(
                fc::time_point_sec when,
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
                fc::time_point_sec when,
                const account_name& builder
        )
        {
            try {

                auto start = fc::time_point::now();

                signed_block pending_block;
                const auto& gprops = get_static_config();

                uint32_t pending_block_size = fc::raw::pack_size( pending_block );

                pending_block.previous = get_dynamic_states().head_block_id;
                pending_block.timestamp = when;
                pending_block.transaction_merkle_root = pending_block.calculate_merkle_root();

                pending_block.builder = builder;

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

                next_block.transaction_merkle_root == next_block.calculate_merkle_root();


                update_dynamic_states(next_block);



            } FC_CAPTURE_AND_RETHROW( (next_block.block_num()) )  }


        void chain_xmax::update_dynamic_states(const signed_block& b) {
            const dynamic_states_object& _dgp = _data.get<dynamic_states_object>();

            // update dynamic states
            _data.modify( _dgp, [&]( dynamic_states_object& dgp ){
                dgp.head_block_number = b.block_num();
                dgp.head_block_id = b.id();
                dgp.time = b.timestamp;
                dgp.current_builder = b.builder;
            });

        }

} }
