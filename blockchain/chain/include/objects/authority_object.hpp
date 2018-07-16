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
		account_name		owner_name;
		authority_name		auth_name;

		id_type				parent;
		mapped_authoritys	authoritys;
		time				last_updated;
	};

	struct by_owner;
	using permission_index = Basechain::shared_multi_index_container<
		authority_object,
		indexed_by<
		ordered_unique<tag<by_id>, member<authority_object, authority_object::id_type, &authority_object::id>>,
		ordered_unique<tag<by_owner>, 
			composite_key<authority_object,
				member<authority_object, account_name, &authority_object::owner_name>,
				member<authority_object, authority_name, &authority_object::auth_name>
			>
		>
		>
	>;
}
}


BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::authority_object, Xmaxplatform::Chain::permission_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::authority_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::authority_object, (id)(owner_name)(parent)(auth_name)(authoritys)(last_updated))