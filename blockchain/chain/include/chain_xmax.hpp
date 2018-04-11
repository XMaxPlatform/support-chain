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
#include <blockchain_setup.hpp>
#include <objects/static_config_object.hpp>
#include <objects/dynamic_states_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/account_object.hpp>
#include <transaction.hpp>
#include <message_context_xmax.hpp>

namespace Xmaxplatform { namespace Chain {
   using database = Basechain::database;
        class chain_init;
        class message_xmax;
   class chain_xmax {
      public:

         chain_xmax(database& database,chain_init& init);
         chain_xmax(const chain_xmax&) = delete;
         chain_xmax(chain_xmax&&) = delete;
         chain_xmax& operator=(const chain_xmax&) = delete;
         chain_xmax& operator=(chain_xmax&&) = delete;
         ~chain_xmax();

       const static_config_object&          get_static_config()const;
       const dynamic_states_object&         get_dynamic_states()const;


       time_point           head_block_time() const;

   private:

       database&                        _data;
       bool                             _currently_applying_block = false;
       uint64_t                         _skip_flags = 0;
       typedef pair<account_name,Basetypes::name> handler_key;

       map< account_name, map<handler_key, msg_handler> >                   message_handlers;

       void setup_data_indexes();
       void initialize_chain(chain_init& initer);

       signed_block _generate_block(
               fc::time_point_sec when,
               const account_name& builder
       );

       bool _push_block(const signed_block& new_block);
       void _apply_block(const signed_block& next_block);



           void process_message(const transaction& trx, account_name code, const message_xmax& message,
                            message_output& output, message_context_xmax* parent_context = nullptr,
                            int depth = 0, const fc::time_point& start_time = fc::time_point::now());
       void apply_message(message_context_xmax& c);

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

       template<typename Function>
       auto with_skip_flags( uint64_t flags, Function&& f ) -> decltype((*((Function*)nullptr))())
       {
           auto old_flags = _skip_flags;
           auto on_exit   = fc::make_scoped_exit( [&](){ _skip_flags = old_flags; } );
           _skip_flags = flags;
           return f();
       }

       void set_message_handler( const account_name& contract, const account_name& scope, const action_name& action, msg_handler v );


       void update_dynamic_states(const signed_block& b);
       const Basechain::database& get_data() const { return _data; }
   };

    class message_xmax;
        class chain_init {
        public:
            virtual ~chain_init();


            virtual Basetypes::time get_chain_init_time() = 0;

            virtual Chain::blockchain_setup get_blockchain_setup() = 0;

            virtual std::array<account_name, Config::blocks_per_round> get_chain_init_builders() = 0;

            virtual void register_handlers(chain_xmax &chain, database &db) = 0;

            virtual vector<message_xmax> prepare_data(chain_xmax &chain, database &db) = 0;
        };

} }
