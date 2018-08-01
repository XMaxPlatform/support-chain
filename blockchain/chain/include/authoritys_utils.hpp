/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <authoritys.hpp>
namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		void check_authorization(const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys);

		bool validate_weight(const authority& auth);

		const authority_object& get_permission(database& db, const account_auth& auth);
	}

}
}