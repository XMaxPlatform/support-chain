/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <blockchain_types.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform { namespace Chain {

   struct by_scope_primary;
   struct by_scope_secondary;
   struct by_scope_tertiary;

   struct key_value_object : public Basechain::object<key_value_object_type, key_value_object> {
	   OBJECT_CCTOR(key_value_object, (value))
      
      typedef uint128 key_type;
      static const int number_of_keys = 1;

      id_type               id;
      account_name          scope;
      account_name          code;
      account_name          table;
      uint128              primary_key;
      mapped_string         value;
   };

   using key_value_index = Basechain::shared_multi_index_container<
      key_value_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<key_value_object, key_value_object::id_type, &key_value_object::id>>,
         ordered_unique<tag<by_scope_primary>, 
            composite_key< key_value_object,
               member<key_value_object, account_name, &key_value_object::scope>,
               member<key_value_object, account_name, &key_value_object::code>,
               member<key_value_object, account_name, &key_value_object::table>,
               member<key_value_object, uint128, &key_value_object::primary_key>
            >,
            composite_key_compare< std::less<account_name>,std::less<account_name>,std::less<account_name>,std::less<uint128> >
         >
      >
   >;

   struct shared_string_less {
      bool operator()( const char* a, const char* b )const {
         return less(a, b);
      }

      bool operator()( const std::string& a, const char* b )const {
         return less(a.c_str(), b);
      }

      bool operator()( const char* a, const std::string& b )const {
         return less(a, b.c_str());
      }

      inline bool less( const char* a, const char* b )const{
         return std::strcmp( a, b ) < 0;
      }
   };

   struct keystr_value_object : public Basechain::object<keystr_value_object_type, keystr_value_object> {
      OBJECT_CCTOR(keystr_value_object, (primary_key)(value))
      
      typedef std::string key_type;
      static const int number_of_keys = 1;

      const char* data() const { return primary_key.data(); }

      id_type               id;
      account_name          scope;
      account_name          code;
      account_name          table;
      mapped_string         primary_key;
      mapped_string         value;
   };

   using keystr_value_index = Basechain::shared_multi_index_container<
      keystr_value_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<keystr_value_object, keystr_value_object::id_type, &keystr_value_object::id>>,
         ordered_unique<tag<by_scope_primary>, 
            composite_key< keystr_value_object,
               member<keystr_value_object, account_name, &keystr_value_object::scope>,
               member<keystr_value_object, account_name, &keystr_value_object::code>,
               member<keystr_value_object, account_name, &keystr_value_object::table>,
               const_mem_fun<keystr_value_object, const char*, &keystr_value_object::data>
            >,
            composite_key_compare< std::less<account_name>,std::less<account_name>,std::less<account_name>,shared_string_less>
         >
      >
   >;

   struct key128x128_value_object : public Basechain::object<key128x128_value_object_type, key128x128_value_object> {
      OBJECT_CCTOR(key128x128_value_object, (value))

      typedef uint128 key_type;
      static const int number_of_keys = 2;

      id_type               id;
      account_name          scope;
      account_name          code;
      account_name          table;
	  key_type             primary_key;
	  key_type             secondary_key;
      mapped_string         value;
   };

   using key128x128_value_index = Basechain::shared_multi_index_container<
      key128x128_value_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<key128x128_value_object, key128x128_value_object::id_type, &key128x128_value_object::id>>,
         ordered_unique<tag<by_scope_primary>, 
            composite_key< key128x128_value_object,
               member<key128x128_value_object, account_name, &key128x128_value_object::scope>,
               member<key128x128_value_object, account_name, &key128x128_value_object::code>,
               member<key128x128_value_object, account_name, &key128x128_value_object::table>,
               member<key128x128_value_object, key128x128_value_object::key_type, &key128x128_value_object::primary_key>,
               member<key128x128_value_object, key128x128_value_object::key_type, &key128x128_value_object::secondary_key>
            >,
            composite_key_compare< std::less<account_name>,std::less<account_name>,std::less<account_name>,std::less<key128x128_value_object::key_type>,std::less<key128x128_value_object::key_type> >
         >,
         ordered_unique<tag<by_scope_secondary>, 
            composite_key< key128x128_value_object,
               member<key128x128_value_object, account_name, &key128x128_value_object::scope>,
               member<key128x128_value_object, account_name, &key128x128_value_object::code>,
               member<key128x128_value_object, account_name, &key128x128_value_object::table>,
               member<key128x128_value_object, key128x128_value_object::key_type, &key128x128_value_object::secondary_key>,
               member<key128x128_value_object, key128x128_value_object::key_type, &key128x128_value_object::primary_key>
            >,
            composite_key_compare< std::less<account_name>,std::less<account_name>,std::less<account_name>,std::less<key128x128_value_object::key_type>,std::less<key128x128_value_object::key_type> >
         >
      >
   >;

   struct key128x128x128_value_object : public Basechain::object<key128x128x128_value_object_type, key128x128x128_value_object> {
	   OBJECT_CCTOR(key128x128x128_value_object, (value))

		   typedef uint128 key_type;
	   static const int number_of_keys = 3;

	   id_type               id;
	   account_name          scope;
	   account_name          code;
	   account_name          table;
	   uint128              primary_key;
	   uint128              secondary_key;
	   uint128              tertiary_key;
	   mapped_string         value;
   };

   using key128x128x128_value_index = Basechain::shared_multi_index_container <
	   key128x128x128_value_object,
	   indexed_by <
	   ordered_unique<tag<by_id>, member<key128x128x128_value_object, key128x128x128_value_object::id_type, &key128x128x128_value_object::id>>,
	   ordered_unique<tag<by_scope_primary>,
	   composite_key< key128x128x128_value_object,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::scope>,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::code>,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::table>,
	   member<key128x128x128_value_object, uint128, &key128x128x128_value_object::primary_key>,
	   member<key128x128x128_value_object, uint128, &key128x128x128_value_object::secondary_key>,
	   member<key128x128x128_value_object, uint128, &key128x128x128_value_object::tertiary_key>
	   >,
	   composite_key_compare< std::less<account_name>, std::less<account_name>, std::less<account_name>, std::less<uint128>, std::less<uint128>, std::less<uint128> >
	   >,
	   ordered_unique<tag<by_scope_secondary>,
	   composite_key< key128x128x128_value_object,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::scope>,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::code>,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::table>,
	   member<key128x128x128_value_object, uint128, &key128x128x128_value_object::secondary_key>,
	   member<key128x128x128_value_object, uint128, &key128x128x128_value_object::tertiary_key>/*,
	   member<key128x128x128_value_object, typename key128x128x128_value_object::id_type, &key128x128x128_value_object::id>*/
	   >
	   >,
	   ordered_unique<tag<by_scope_tertiary>,
	   composite_key< key128x128x128_value_object,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::scope>,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::code>,
	   member<key128x128x128_value_object, account_name, &key128x128x128_value_object::table>,
	   member<key128x128x128_value_object, uint128, &key128x128x128_value_object::tertiary_key>/*,
	   member<key128x128x128_value_object, typename key128x128x128_value_object::id_type, &key128x128x128_value_object::id>*/
	   >,
	   composite_key_compare< std::less<account_name>, std::less<account_name>, std::less<account_name>, std::less<uint128>/*, std::less<typename key128x128x128_value_object::id_type>*/ >
	   >
	   >
   > ;

   struct key64x64x64_value_object : public Basechain::object<key64x64x64_value_object_type, key64x64x64_value_object> {
      OBJECT_CCTOR(key64x64x64_value_object, (value))

      typedef uint64_t key_type;
      static const int number_of_keys = 3;

      id_type               id;
      account_name          scope;
      account_name          code;
      account_name          table;
      uint64_t              primary_key;
      uint64_t              secondary_key;
      uint64_t              tertiary_key;
      mapped_string         value;
   };

   using key64x64x64_value_index = Basechain::shared_multi_index_container<
      key64x64x64_value_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<key64x64x64_value_object, key64x64x64_value_object::id_type, &key64x64x64_value_object::id>>,
         ordered_unique<tag<by_scope_primary>,
            composite_key< key64x64x64_value_object,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::scope>,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::code>,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::table>,
               member<key64x64x64_value_object, uint64_t, &key64x64x64_value_object::primary_key>,
               member<key64x64x64_value_object, uint64_t, &key64x64x64_value_object::secondary_key>,
               member<key64x64x64_value_object, uint64_t, &key64x64x64_value_object::tertiary_key>
            >,
            composite_key_compare< std::less<account_name>,std::less<account_name>,std::less<account_name>,std::less<uint64_t>,std::less<uint64_t>,std::less<uint64_t> >
         >,
         ordered_unique<tag<by_scope_secondary>,
            composite_key< key64x64x64_value_object,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::scope>,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::code>,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::table>,
               member<key64x64x64_value_object, uint64_t, &key64x64x64_value_object::secondary_key>,
               member<key64x64x64_value_object, uint64_t, &key64x64x64_value_object::tertiary_key>,
               member<key64x64x64_value_object, typename key64x64x64_value_object::id_type, &key64x64x64_value_object::id>
            >
         >,
         ordered_unique<tag<by_scope_tertiary>,
            composite_key< key64x64x64_value_object,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::scope>,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::code>,
               member<key64x64x64_value_object, account_name, &key64x64x64_value_object::table>,
               member<key64x64x64_value_object, uint64_t, &key64x64x64_value_object::tertiary_key>,
               member<key64x64x64_value_object, typename key64x64x64_value_object::id_type, &key64x64x64_value_object::id>
            >,
            composite_key_compare< std::less<account_name>,std::less<account_name>,std::less<account_name>,std::less<uint64_t>,std::less<typename key64x64x64_value_object::id_type> >
         >         
      >
   >;




} } // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::key_value_object, Xmaxplatform::Chain::key_value_index)
BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::keystr_value_object, Xmaxplatform::Chain::keystr_value_index)
BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::key128x128_value_object, Xmaxplatform::Chain::key128x128_value_index)
BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::key64x64x64_value_object, Xmaxplatform::Chain::key64x64x64_value_index)
BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::key128x128x128_value_object, Xmaxplatform::Chain::key128x128x128_value_index)

FC_REFLECT(Xmaxplatform::Chain::key_value_object, (id)(scope)(code)(table)(primary_key)(value) )
