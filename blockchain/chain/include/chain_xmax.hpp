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
   using boost::signals2::signal;
        class chain_init;
        class message_xmax;
		class builder_object;
   class chain_xmax {
      public:

         chain_xmax(database& database,chain_init& init);
         chain_xmax(const chain_xmax&) = delete;
         chain_xmax(chain_xmax&&) = delete;
         chain_xmax& operator=(const chain_xmax&) = delete;
         chain_xmax& operator=(chain_xmax&&) = delete;
         ~chain_xmax();

		 signal<void(const signed_transaction&)> on_pending_transaction;

		 enum validation_steps
		 {
			 skip_nothing = 0,
			 skip_producer_signature = 1 << 0,  ///< used while reindexing
			 skip_transaction_signatures = 1 << 1,  ///< used by non-producer nodes
			 skip_transaction_dupe_check = 1 << 2,  ///< used while reindexing
			 skip_fork_db = 1 << 3,  ///< used while reindexing
			 skip_block_size_check = 1 << 4,  ///< used when applying locally generated transactions
			 skip_tapos_check = 1 << 5,  ///< used while reindexing -- note this skips expiration check as well
			 skip_authority_check = 1 << 6,  ///< used while reindexing -- disables any checking of authority on transactions
			 skip_merkle_check = 1 << 7,  ///< used while reindexing
			 skip_assert_evaluation = 1 << 8,  ///< used while reindexing
			 skip_undo_history_check = 1 << 9,  ///< used while reindexing
			 skip_producer_schedule_check = 1 << 10, ///< used while reindexing
			 skip_validate = 1 << 11, ///< used prior to checkpoint, skips validate() call on transaction
			 skip_scope_check = 1 << 12, ///< used to skip checks for proper scope
			 skip_output_check = 1 << 13, ///< used to skip checks for outputs in block exactly matching those created from apply
			 pushed_transaction = 1 << 14, ///< used to indicate that the origination of the call was from a push_transaction, to determine time allotment
			 created_block = 1 << 15, ///< used to indicate that the origination of the call was for creating a block, to determine time allotment
			 received_block = 1 << 16, ///< used to indicate that the origination of the call was for a received block, to determine time allotment
			 irreversible = 1 << 17  ///< indicates the block was received while catching up and is already considered irreversible.
		 };

       const static_config_object&          get_static_config()const;
       const dynamic_states_object&         get_dynamic_states()const;

       time             head_block_time() const;
	   uint32_t			head_block_num() const;
	   xmax_type_block_id    head_block_id()const;

	   const builder_info&         get_block_builder(uint32_t delta_slot) const;
	   const builder_info&         get_order_builder(uint32_t order_slot) const;
	   const builder_object*	find_builder_object(account_name builder_name) const;
	   uint32_t				get_delta_slot_at_time(chain_timestamp when) const;
       chain_timestamp		get_delta_slot_time(uint32_t delta_slot) const;

	   const Basechain::database& get_database() const { return _data; }
	   Basechain::database& get_mutable_database() { return _data; }

	   vector<char>       message_to_binary(name code, name type, const fc::variant& obj)const;
	   fc::variant        message_from_binary(name code, name type, const vector<char>& bin)const;

	   processed_transaction push_transaction(const signed_transaction& trx, uint32_t skip = skip_nothing);
	   processed_transaction _push_transaction(const signed_transaction& trx);
   private:

       database&                        _data;
       bool                             _currently_applying_block = false;
	   const uint32_t                   _pending_txn_depth_limit;
       uint64_t                         _skip_flags = 0;
       typedef pair<account_name,Basetypes::name> handler_key;

       map< account_name, map<handler_key, msg_handler> >                   message_handlers;

	   optional<database::session>      _pending_tx_session;
	   deque<signed_transaction>         _pending_transactions;

       void setup_data_indexes();
       void initialize_chain(chain_init& initer);

       signed_block _generate_block(
               chain_timestamp when,
               const account_name& builder
       );
	   bool push_block(const signed_block& b);
       bool _push_block(const signed_block& new_block);
       void _apply_block(const signed_block& next_block);
	   void _finalize_block(const signed_block& b);
	 // void rate_limit_message(const message& message);
      void process_message(const transaction& trx, account_name code, const message_xmax& message,
                            message_output& output, message_context_xmax* parent_context = nullptr,
                            int depth = 0, const fc::time_point& start_time = fc::time_point::now());
       void apply_message(message_context_xmax& c);

	   template<typename T>
	   typename T::processed apply_transaction(const T& trx);
	   template<typename T>
	   typename T::processed process_transaction(const T& trx, int depth, const fc::time_point& start_time);


	   void require_account(const account_name& name) const;

	   void validate_uniqueness(const Chain::signed_transaction& trx)const;
	   void validate_uniqueness(const generated_transaction& trx)const;
	   void validate_tapos(const transaction& trx)const;
	   void validate_referenced_accounts(const transaction& trx)const;
	   void validate_expiration(const transaction& trx) const;
	   void validate_scope(const transaction& trx) const;

	   void record_transaction(const signed_transaction& trx);
	   void record_transaction(const generated_transaction& trx);


	   bool should_check_for_duplicate_transactions()const { return !(_skip_flags&skip_transaction_dupe_check); }
	   bool should_check_tapos()const { return !(_skip_flags&skip_tapos_check); }


	   void check_transaction_authorization(const signed_transaction& trx, bool allow_unused_signatures = false)const;
	   void create_block_summary(const signed_block& next_block);
	   void update_or_create_builders(const builder_rule& builders);
	   template<typename T>
	   void validate_transaction(const T& trx) const {
		   try {

			   XMAX_ASSERT(trx.messages.size() > 0, transaction_exception, "A transaction must have at least one message");

			   validate_scope(trx);
			   validate_expiration(trx);
			   validate_uniqueness(trx);
			   validate_tapos(trx);

		   } FC_CAPTURE_AND_RETHROW((trx))
	   }

	   template<typename Function>
	   auto with_applying_block(Function&& f) -> decltype((*((Function*)nullptr))()) {
		   auto on_exit = fc::make_scoped_exit([this]() {
			   _currently_applying_block = false;
		   });
		   _currently_applying_block = true;
		   return f();
	   }

	   template<typename Function>
	   auto with_skip_flags(uint64_t flags, Function&& f) -> decltype((*((Function*)nullptr))())
	   {
		   auto old_flags = _skip_flags;
		   auto on_exit = fc::make_scoped_exit([&]() { _skip_flags = old_flags; });
		   _skip_flags = flags;
		   return f();
	   }


   public:
	   xmax_type_block_id               get_blockid_from_num(uint32_t block_num)const;
	   optional<signed_block>			get_block_from_id(const xmax_type_block_id& id)const;
	   optional<signed_block>      get_block_from_num(uint32_t num)const;

       signed_block generate_block(
               chain_timestamp when,
               const account_name& builder,
			   const private_key_type sign_private_key
       );

       void apply_block(const signed_block& next_block);

       void set_message_handler( const account_name& contract, const account_name& scope, const action_name& action, msg_handler v );


       const Basechain::database& get_data() const { return _data; }


   };

    class message_xmax;
        class chain_init {
        public:
            virtual ~chain_init();


            virtual Basetypes::time get_chain_init_time() const = 0;

            virtual Chain::blockchain_setup get_blockchain_setup() const = 0;

            virtual Chain::xmax_builder_infos get_chain_init_builders() const = 0;

            virtual void register_handlers(chain_xmax &chain, database &db) = 0;

            virtual vector<message_xmax> prepare_data(chain_xmax &chain, database &db) = 0;
        };

} }
