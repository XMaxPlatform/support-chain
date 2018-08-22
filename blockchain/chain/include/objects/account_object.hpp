/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>
#include <account_type.hpp>
#include <mapped_ptr.hpp>
#include <objects/account_contract.hpp>
#include "multi_index_includes.hpp"

namespace Xmaxplatform { namespace Chain {

   class account_object : public Basechain::object<account_object_type, account_object> {
	   OBJECT_CCTOR(account_object, (contract))
   public:
	  using acc_type = fc::enum_type<uint8_t, account_type>;
      id_type             id;
      account_name        name;
	  acc_type			  type = 0;
      time                creation_date;

	  mapped_ptr<account_contract> contract;

	  template<typename T>
	  void set_contract(const T& code, const Xmaxplatform::Basetypes::abi& abi)
	  {
		  set_contract((const char*)(code.data()), code.size(), abi);
	  }

   private:
	   void set_contract(const char* code, size_t len, const Xmaxplatform::Basetypes::abi& abi);
   };
   using account_id_type = account_object::id_type;

   struct by_name;
   using account_index = Basechain::shared_multi_index_container<
      account_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_object, account_object::id_type, &account_object::id>>,
         ordered_unique<tag<by_name>, member<account_object, account_name, &account_object::name>>
      >
   >;

} } // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::account_object, Xmaxplatform::Chain::account_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::account_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::account_object, (id)(name)(type)(creation_date)(contract))
