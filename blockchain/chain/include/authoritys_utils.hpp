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

		bool validate_weight_format(const authority& auth);

		const authority_object& get_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth);

		const authority_object* find_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth);

		void modify_authority_object(Basechain::database& db, const authority_object& obj, const Basetypes::authority& new_auth, time t);

		const authority_object& new_authority_object(Basechain::database& db, account_name account, authority_name name,
			authority_object::id_type parent, authority&& auth, time t);

		void remove_authority_object(Basechain::database& db, const authority_object& obj);

		bool check_authority_name(authority_name name);
	}

}
}