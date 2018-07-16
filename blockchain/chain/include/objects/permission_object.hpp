/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>

#include "multi_index_includes.hpp"

namespace Xmaxplatform {
namespace Chain {

	class permission_object : public Basechain::object<permission_object_type, permission_object> {
		OBJECT_CCTOR(permission_object)
	public:
		id_type                           id;
		id_type                           parent;
		account_name                      account;
		permission_name                   name;
	}
}
}