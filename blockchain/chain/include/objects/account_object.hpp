/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>

#include "multi_index_includes.hpp"




namespace Xmaxplatform {
namespace Chain {
	enum account_type : uint8_t
	{
		acc_personal = 0,
		acc_contract = 1,
		acc_erc20 = 10,
		acc_erc721 = 11,
	};

}
}
FC_REFLECT_ENUM(Xmaxplatform::Chain::account_type, (acc_personal)(acc_contract)(acc_erc20)(acc_erc721))



namespace Xmaxplatform { namespace Chain {

   class account_object : public Basechain::object<account_object_type, account_object> {
      OBJECT_CCTOR(account_object,(code)(abi))
   public:
	  using acc_type = fc::enum_type<uint8_t, account_type>;
      id_type             id;
      account_name        name;
	  acc_type			  type = 0;
      time                creation_date;
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

FC_REFLECT(Xmaxplatform::Chain::account_object, (id)(name)(type)(creation_date))
