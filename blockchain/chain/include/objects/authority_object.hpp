/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <authoritys.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform {
namespace Chain {

	class authority_object : public Basechain::object<authority_object_type, authority_object> {
		OBJECT_CCTOR(authority_object, (authoritys))
	public:
		id_type				id;	
		account_name		owner_name;		// the name of owner account.
 		authority_name		auth_name;		// the name of this authority object.

		id_type				parent;			// the id of parent authority object.
		mapped_authoritys	authoritys;		// config of authorization.
		time				last_updated;
	};

	struct by_owner;
	struct by_name;
	using authority_index = Basechain::shared_multi_index_container<
		authority_object,
		indexed_by<
		ordered_unique<tag<by_id>, member<authority_object, authority_object::id_type, &authority_object::id>>,
		ordered_unique<tag<by_owner>, 
			composite_key<authority_object,
				member<authority_object, account_name, &authority_object::owner_name>,
				member<authority_object, authority_name, &authority_object::auth_name>
			>,
		ordered_unique<tag<by_name>,
			composite_key<authority_object,
				member<authority_object, account_name, &authority_object::owner_name>,
				member<authority_object, authority_object::id_type, &authority_object::id>
			>
		>	
		>
		>
	>;
}
}


BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::authority_object, Xmaxplatform::Chain::authority_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::authority_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::authority_object, (id)(owner_name)(parent)(auth_name)(authoritys)(last_updated))