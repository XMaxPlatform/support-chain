/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <authoritys_utils.hpp>
namespace Xmaxplatform {
namespace Chain {

	using Basechain::database;

	namespace utils
	{
		void check_authorization(const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys)
		{
			for (const Basetypes::message& msg : messages)
			{
				if (msg.code == Config::xmax_contract_name)
				{

				}

			}
		}

		bool validate_weight(const authority& auth)
		{
			auth_weight weights = 0;

			if ((auth.keys.size() + auth.accounts.size()) > (1 << 16))
				return false; // (assumes auth_weight is uint16_t and threshold is of type uint32_t)

			if (auth.threshold == 0)
				return false;

			{
				const key_permission_weight* prev = nullptr;
				for (const auto& k : auth.keys) {
					if (prev && !(prev->key < k.key)) return false; // TODO: keys must be sorted.
					weights += k.weight;
					prev = &k;
				}
			}

			{
				const account_permission_weight* prev = nullptr;
				for (const auto& a : auth.accounts) {
					if (prev && (prev->permission >= a.permission)) return false; // TODO: accounts must be sorted.
					weights += a.weight;
					prev = &a;
				}
			}
			return weights >= auth.threshold;
		}

		const authority_object& get_permission(database& db, const account_auth& auth)
		{
			try {
				FC_ASSERT(!auth.account.empty() && !auth.permission.empty(), "Invalid authority");

				return db.get<authority_object, by_owner>(std::make_tuple(auth.account, auth.permission));

			} FC_CAPTURE_AND_RETHROW(("auth", auth))
		}



	}
}
}