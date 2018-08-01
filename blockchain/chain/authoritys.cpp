/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <authoritys.hpp>
namespace Xmaxplatform {
namespace Chain {

	void check_authorization(const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys)
	{
		for (const Basetypes::message& msg : messages)
		{
			if (msg.code == Config::xmax_contract_name)
			{

			}

		}
	}

	bool validate_authorization(const authority& auth)
	{
		auth_weight weights = 0;

		if ((auth.keys.size() + auth.accounts.size() + auth.waits.size()) > (1 << 16))
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
}
}