/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once


#include <fc/scoped_exit.hpp>

#include <boost/signals2/signal.hpp>

#include <fc/log/logger.hpp>

#include <map>
#include <blockchain_types.hpp>
#include <block.hpp>
#include <block_pack.hpp>
#include <blockchain_setup.hpp>
#include <objects/static_config_object.hpp>
#include <objects/dynamic_states_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/account_object.hpp>
#include <transaction.hpp>
#include <transaction_request.hpp>
#include <message_context_xmax.hpp>

namespace Xmaxplatform { namespace Chain {
   using boost::signals2::signal;
   using database = Basechain::database;
   using finalize_block_func = fc::optional<signal<void(const signed_block&)>::slot_type>;
   
   class chain_init;
   struct message_xmax;
   class builder_object;
   class chain_context;

   class chain_xmax {

   public:

	   struct xmax_config
	   {
		   bool open_flag = false;
		   uint64_t  shared_memory_size = 0;
		   Basechain::bfs::path block_memory_dir;
		   Basechain::bfs::path fork_memory_dir;
		   Basechain::bfs::path block_log_dir;
		   Chain::chain_id_type      chain_id;
		   uint32_t	skip_flags = Config::skip_nothing;
	   };

      public:

         chain_xmax(chain_init& init, const xmax_config& config, const finalize_block_func& finalize_func);
         chain_xmax(const chain_xmax&) = delete;
         chain_xmax(chain_xmax&&) = delete;
         chain_xmax& operator=(const chain_xmax&) = delete;
         chain_xmax& operator=(chain_xmax&&) = delete;
         ~chain_xmax();

		 signal<void(const signed_transaction&)> on_pending_transaction;
		 signal<void(const signed_block&)> on_finalize_block;

       const static_config_object&          get_static_config()const;
       const dynamic_states_object&         get_dynamic_states()const;

       time             head_block_time() const;
	   uint32_t			head_block_num() const;
	   uint32_t			last_irreversible_block_num() const;
	   xmax_type_block_id    head_block_id()const;

	   const builder_info&         get_block_builder(uint32_t delta_slot) const;
	   const builder_info&         get_order_builder(uint32_t order_slot) const;
	   const builder_object*	find_builder_object(account_name builder_name) const;
	   uint32_t				get_delta_slot_at_time(chain_timestamp when) const;
       chain_timestamp		get_delta_slot_time(uint32_t delta_slot) const;

	   const Basechain::database& get_database() const;
	   Basechain::database& get_mutable_database();

	   vector<char>       message_to_binary(name code, name type, const fc::variant& obj)const;
	   fc::variant        message_from_binary(name code, name type, const vector<char>& bin)const;
	   fc::variant        event_from_binary(name code, type_name tname, const vector<char>& bin)const;

	   processed_transaction transaction_from_variant(const fc::variant& v)const;
	   fc::variant       transaction_to_variant(const processed_transaction& trx)const;
	   fc::variant       transaction_events_to_variant(const processed_transaction& trx)const;

	   processed_transaction push_transaction(const signed_transaction& trx, uint32_t skip = Config::skip_nothing);
	   processed_transaction _push_transaction(transaction_request_ptr request);

	   flat_set<public_key_type> get_required_keys(const signed_transaction& transaction, const flat_set<public_key_type>& candidateKeys)const;
   private:

	   std::unique_ptr<chain_context>   _context;

       void setup_data_indexes();
       void initialize_chain(chain_init& initer);

	   void _abort_build();

	   void _start_build(chain_timestamp when);

       void _build_block(const private_key_type& sign_private_key);

	   void _commit_block();

       void _apply_block(const signed_block& next_block);

	   void _update_final_state(const signed_block& b);

	   void _irreversible_block(const block_pack_ptr& pack);
	 // void rate_limit_message(const message& message);
      void process_message(const transaction& trx, account_name code, const message_xmax& message,
                            message_output& output, message_context_xmax* parent_context = nullptr,
                            int depth = 0, const fc::time_point& start_time = fc::time_point::now());
       void apply_message(message_context_xmax& c);

	   template<typename T>
	   typename T::processed apply_transaction(const T& trx);
	   template<typename T>
	   typename T::processed process_transaction(const T& trx, int depth, const fc::time_point& start_time);

	   void process_confirmation(const block_confirmation& conf);

	   void require_account(const account_name& name) const;

	   void validate_uniqueness(const Chain::signed_transaction& trx)const;
	   void validate_uniqueness(const generated_transaction& trx)const;
	   void validate_tapos(const transaction& trx)const;
	   void validate_referenced_accounts(const transaction& trx)const;
	   void validate_expiration(const transaction& trx) const;
	   void validate_scope(const transaction& trx) const;

	   void record_transaction(const signed_transaction& trx);
	   void record_transaction(const generated_transaction& trx);


	   bool should_check_for_duplicate_transactions()const;
	   bool should_check_tapos()const;


	   void check_transaction_authorization(const signed_transaction& trx, bool allow_unused_signatures = false)const;
	   void block_summary(const signed_block& next_block);
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

   public:
	   xmax_type_block_id               get_blockid_from_num(uint32_t block_num)const;
	   optional<signed_block>			get_block_from_id(const xmax_type_block_id& id)const;
	   optional<signed_block>      get_block_from_num(uint32_t num)const;

       void build_block(
               chain_timestamp when,
			   const private_key_type& sign_private_key
       );

       void apply_block(const signed_block& next_block);

       void set_message_handler( const account_name& contract, const account_name& scope, const action_name& action, msg_handler v );


   };

} }
