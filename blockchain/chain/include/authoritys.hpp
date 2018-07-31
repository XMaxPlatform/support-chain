/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
namespace Xmaxplatform {
namespace Chain {

	typedef uint16 auth_weight;

	struct permission_weight
	{
	public:
		permission_weight(auth_weight w)
			: weight(w)
		{

		}

		auth_weight weight;
	};


	struct key_permission : public permission_weight
	{
		key_permission(auth_weight w, const public_key_type& pk)
			: permission_weight(w)
			, key(pk)
		{

		}
		public_key_type key;
	};

	struct acc_auth
	{
		acc_auth(account_name acc, authority_name per)
			: account(acc), permission(per)
		{

		}
		account_name account;
		authority_name permission;
	};

	struct account_permission : public permission_weight
	{
		account_permission(auth_weight w, account_name acc, authority_name per)
			: permission_weight(w)
			, auth(acc, per)
		{

		}
		acc_auth auth;
	};


	struct mapped_authoritys
	{
		mapped_authoritys(Basechain::allocator<char> alloc)
			: keys(alloc)
			, accounts(alloc)
		{

		}

		void operator = (const Xmaxplatform::Basetypes::authority& auths)
		{
			this->threshold = auths.threshold;
			for (const auto& it : auths.keys)
			{
				this->keys.emplace_back(key_permission(it.weight, it.key));
			}
			for (const auto& it : auths.accounts)
			{
				this->accounts.emplace_back(account_permission(it.weight, it.permission.account, it.permission.authority));
			}
		}
		uint32								threshold;
		mapped_vector<key_permission>		keys;
		mapped_vector<account_permission>	accounts;
	};


	void check_authorization(const std::vector<Basetypes::message>& messages, const flat_set<public_key>& keys);
}
}