/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>
#include "multi_index_includes.hpp"


namespace Xmaxplatform { namespace Chain {

   class contract_object : public Basechain::object<contract_object_type, contract_object> {
      OBJECT_CCTOR(contract_object,(code)(abi))

      id_type             id;
      account_name        name;
	  fc::sha256          code_version;
	  mapped_vector<char> code;
	  mapped_vector<char> abi;

	  void set_abi(const Xmaxplatform::Basetypes::abi& _abi);
   };
   using contract_id_type = contract_object::id_type;

   struct by_name;
   using contract_index = Basechain::shared_multi_index_container<
      contract_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<contract_object, contract_object::id_type, &contract_object::id>>,
         ordered_unique<tag<by_name>, member<contract_object, account_name, &contract_object::name>>
      >
   >;

} } // Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::contract_object, Xmaxplatform::Chain::contract_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::contract_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::contract_object, (id)(name)(code_version)(code)(abi))
