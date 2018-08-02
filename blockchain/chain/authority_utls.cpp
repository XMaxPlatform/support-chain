/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <objects/linked_permission_object.hpp>
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

		bool validate_weight_format(const authority& auth)
		{
			auth_weight weights = 0;

			if ((auth.keys.size() + auth.accounts.size()) > (1 << 16))
				return false; // (assumes auth_weight is uint16_t and threshold is of type uint32_t)

			if (auth.threshold == 0)
				return false;

			if (!auth.keys.empty())
			{
				weights += auth.keys[0].weight;
				for (int i = 1; i < auth.keys.size(); ++i)
				{
					if (!(auth.keys[i - 1].key < auth.keys[i].key))
						return false; // TODO: keys must be sorted.

					weights += auth.keys[i].weight;
				}
			}

			if (!auth.accounts.empty())
			{
				weights += auth.accounts[0].weight;
				for (int i = 1; i < auth.accounts.size(); ++i)
				{
					if (!(auth.accounts[i - 1].permission < auth.accounts[i].permission))
						return false; // TODO: keys must be sorted.

					weights += auth.accounts[i].weight;
				}
			}

			return weights >= auth.threshold;
		}

		const authority_object& get_authority_object(const database& db, const Basetypes::account_auth& auth)
		{
			try {
				FC_ASSERT(!auth.account.empty() && !auth.authority.empty(), "Invalid authority");

				return db.get<authority_object, by_owner>(std::make_tuple(auth.account, auth.authority));

			} FC_CAPTURE_AND_RETHROW(("auth", auth))
		}

		const authority_object* find_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth)
		{
			try {
				FC_ASSERT(!auth.account.empty() && !auth.authority.empty(), "Invalid authority");

				return db.find<authority_object, by_owner>(std::make_tuple(auth.account, auth.authority));

			} FC_CAPTURE_AND_RETHROW(("auth", auth))
		}

		void modify_authority_object(Basechain::database& db, const authority_object& obj, const Basetypes::authority& new_auth, time t)
		{
			db.modify(obj, [&](authority_object& po) {
				po.authoritys = new_auth;
				po.last_updated = t;
			});
		}

		const authority_object& new_authority_object(Basechain::database& db, account_name account, authority_name name,
			authority_object::id_type parent, const authority& auth, time t)
		{
			return db.create<authority_object>([&](authority_object& po) {
				po.owner_name = account;
				po.auth_name = name;
				po.parent = parent;
				po.authoritys = auth;
				po.last_updated = t;
			});
		}

		void remove_authority_object(Basechain::database& db, const authority_object& obj)
		{
			db.remove(obj);
		}

		void remove_linked_object(Basechain::database& db, account_name account, authority_name auth)
		{
			// linked permission
			const auto& index = db.get_index<linked_permission_index, by_permission>();
			auto range = index.equal_range(std::make_tuple(account, auth));
			XMAX_ASSERT(range.first == range.second, message_validate_exception,
				"Cannot delete a linked authority. Please Unlink it first. This authority is linked to ${code}::${type}.",
				("code", string(range.first->code))("type", string(range.first->func)));
			
		}

		bool check_authority_name(authority_name name)
		{
			return name.to_string().find(Config::xmax_contract_string) != 0;
		}



	}
}
}