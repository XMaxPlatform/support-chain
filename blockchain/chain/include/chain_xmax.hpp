/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once


#include <basechain.hpp>
#include <fc/scoped_exit.hpp>

#include <boost/signals2/signal.hpp>

#include <fc/log/logger.hpp>

#include <map>
#include <blockchain_types.hpp>
#include <block.hpp>
#include <objects/static_config_object.hpp>
#include <objects/dynamic_states_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <genesis_state.hpp>

namespace Xmaxplatform { namespace Chain {
   using database = Basechain::database;

   class chain_xmax {
      public:

         chain_xmax(database& database,Native_contract::genesis_state_type& genesis_config);
         chain_xmax(const chain_xmax&) = delete;
         chain_xmax(chain_xmax&&) = delete;
         chain_xmax& operator=(const chain_xmax&) = delete;
         chain_xmax& operator=(chain_xmax&&) = delete;
         ~chain_xmax();

       const static_config_object&          get_static_config()const;
       const dynamic_states_object&         get_dynamic_states()const;

   private:
       Native_contract::genesis_state_type               _genesis_config;
       database&                        _data;
       bool                             _currently_applying_block = false;

       void setup_data_indexes();
       void initialize_chain();

       signed_block _generate_block(
               fc::time_point_sec when,
               const account_name& builder
       );

       bool _push_block(const signed_block& new_block);
       void _apply_block(const signed_block& next_block);

   public:
       signed_block generate_block(
               fc::time_point_sec when,
               const account_name& builder
       );


       bool push_block( const signed_block& b );


       void apply_block(const signed_block& next_block);


       template<typename Function>
       auto with_applying_block(Function&& f) -> decltype((*((Function*)nullptr))()) {
           auto on_exit = fc::make_scoped_exit([this](){
               _currently_applying_block = false;
           });
           _currently_applying_block = true;
           return f();
       }


       void update_dynamic_states(const signed_block& b);
   };

} }
