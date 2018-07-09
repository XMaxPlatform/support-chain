/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>
#include <message_xmax.hpp>

#include <numeric>
using namespace Xmaxplatform::Basetypes;
namespace Xmaxplatform { namespace Chain {
   struct processed_transaction;
   struct inline_transaction;
   struct processed_generated_transaction;
  // xmax_type_summary transaction_digest(const transaction& t);

   void transaction_set_reference_block(transaction& t, const xmax_type_block_id& reference_block);

   bool transaction_verify_reference_block(const transaction& t, const xmax_type_block_id& reference_block);

   template <typename T>
   void transaction_set_message(transaction& t, int index, const Basetypes::func_name& type, T&& value) {
      message_xmax m(t.messages[index]);
      m.set(type, std::forward<T>(value));
      t.messages[index] = m;
   }

   template <typename T>
   T transaction_message_as(transaction& t, int index) {
      message_xmax m(t.messages[index]);
      return m.as<T>();
   }

   template <typename... Args>
   void transaction_emplace_message(transaction& t, Args&&... a) {
      t.messages.emplace_back(message_xmax(std::forward<Args>(a)...));
   }

   template <typename... Args>
   void transaction_emplace_serialized_message(transaction& t, Args&&... a) {
      t.messages.emplace_back(Basetypes::message(std::forward<Args>(a)...));
   }

   /**
      * clear all common data
      */
   inline void transaction_clear(transaction& t) {
      t.messages.clear();
   }


   struct signed_transaction : public Basetypes::signed_transaction {
      typedef Basetypes::signed_transaction super;
      using super::super;

      /// Calculate the id of the transaction
      xmax_type_transaction_id id()const;
      
      /// Calculate the digest used for signature validation
      xmax_type_summary         sig_digest(const chain_id_type& chain_id)const;

      /** signs and appends to signatures */
      const xmax_type_signature& sign(const private_key_type& key, const chain_id_type& chain_id);

      /** returns signature but does not append */
      xmax_type_signature sign(const private_key_type& key, const chain_id_type& chain_id)const;

      flat_set<public_key_type> get_signature_keys(const chain_id_type& chain_id)const;

      /**
       * Removes all messages, signatures, and authorizations
       */
      void clear() { transaction_clear(*this); signatures.clear(); }

      xmax_type_summary merkle_digest() const;

      typedef processed_transaction processed;
   };

   struct pending_inline_transaction : public Basetypes::transaction {
      typedef Basetypes::transaction super;
      using super::super;

      explicit pending_inline_transaction( const Basetypes::transaction& t ):Basetypes::transaction((const Basetypes::transaction& )t){}
      
      typedef inline_transaction processed;
   };

   struct notify_output;
   struct event_output;


   struct event_output {
	   event_name                       name;
	   account_name                     code;
	   func_name                        type;
	   bytes                            data;
	   type_name                        event_type_name;
   };

   struct transaction_package
   {
	   enum package_code
	   {
		   empty = 0,
		   original = 1,
		   zlib = 2, // Reserved Keywords
	   };
	   transaction_package(const signed_transaction& trx)
	   {
		   set_trx(trx);
	   }
	   transaction_package()
		   : code(empty)
	   {

	   }

	   void set_trx(const signed_transaction& trx)
	   {
		   body = trx;
		   code = original;
	   }

	   signed_transaction unpack_trx() const
	   {
		   return body;
	   }

	   package_code code;
	   signed_transaction body;
   };

   typedef std::shared_ptr<transaction_package> transaction_package_ptr;

} } // Xmaxplatform::Chain

FC_REFLECT_DERIVED(Xmaxplatform::Chain::signed_transaction, (Xmaxplatform::Basetypes::signed_transaction), )
FC_REFLECT(Xmaxplatform::Chain::transaction_package, (body))

FC_REFLECT(Xmaxplatform::Chain::event_output, (name)(code)(type)(data))