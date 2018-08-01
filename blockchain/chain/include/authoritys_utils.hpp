/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <authoritys.hpp>
#include <objects/authority_object.hpp>

namespace  Basechain {
	class database;
}

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		void check_authorization(const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys);

		bool validate_weight(const authority& auth);

		const authority_object& get_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth);

		bool check_authority_name(authority_name name);
	}

}
}