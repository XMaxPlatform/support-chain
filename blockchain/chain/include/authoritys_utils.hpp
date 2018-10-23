/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <authoritys.hpp>
#include <objects/authority_object.hpp>
#include <transaction_request.hpp>

namespace  Basechain {
	class database;
}

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		void check_authorization(const Basechain::database& db, const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys, const std::function<void()>&  checktime);

		void check_gaspayer(const Basechain::database& db, transaction_request_ptr transaction);

		bool validate_weight_format(const authority& auth);

		const authority_object& get_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth);

		const authority_object* find_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth);

		void modify_authority_object(Basechain::database& db, const authority_object& obj, const Basetypes::authority& new_auth, time t);

		const authority_object& new_authority_object(Basechain::database& db, account_name account, authority_name name,
			authority_object::id_type parent, const authority& auth, time t);

		void remove_authority_object(Basechain::database& db, const authority_object& obj);

		void remove_linked_object(Basechain::database& db, account_name account, authority_name auth);

		bool check_authority_name(authority_name name);
	}

}
}