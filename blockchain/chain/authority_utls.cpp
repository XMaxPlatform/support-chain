/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <objects/linked_permission_object.hpp>
#include <authoritys_utils.hpp>
#include <objects/xmx_token_object.hpp>


namespace Xmaxplatform {
namespace Chain {

	using Basechain::database;

	static name name_code(const std::string& str)
	{
		return Xmaxplatform::Basetypes::name::to_name_code(str.c_str());
	}
	
	static const func_name updateauth_name = name_code(std::string(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::updateauth>::type().name));
	static const func_name deleteauth_name = name_code(std::string(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::deleteauth>::type().name));
	static const func_name linkauth_name = name_code(std::string(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::linkauth>::type().name));
	static const func_name unlinkauth_name = name_code(std::string(Xmaxplatform::Basetypes::get_struct<Xmaxplatform::Basetypes::unlinkauth>::type().name));

	namespace utils
	{

		const authority_object&  get_authority(const Basechain::database& db, const Basetypes::account_auth& auth)
		{
			try {
				XMAX_ASSERT(!auth.account.empty() && !auth.authority.empty(), authorization_exception, "Invalid permission");
				return db.get<authority_object, by_owner>(boost::make_tuple(auth.account, auth.authority));
			} FC_CAPTURE_AND_RETHROW(authorization_exception, "Failed to retrieve permission: ${auth}", ("auth", auth))
		}

		bool parent_permission_valid(const Basechain::database& db, const authority_object& editor, const authority_object& info)
		{
			if (editor.owner_name != info.owner_name)
				return false;
			if (editor.id == info.parent)
				return true;
			
			// find parent.

			auto& idx = db.get_index<authority_index>();

			const authority_object* parent_auth = idx.find(info.parent);

			while (parent_auth)
			{
				if (parent_auth->parent == editor.id)
				{
					return true;
				}
				if (parent_auth->parent._id == 0)
				{
					return false;
				}

				parent_auth = idx.find(parent_auth->parent);
			}
			return false;
		}

		void check_auth_op(const Basechain::database& db, account_name acc_name, authority_name auth_name, authority_name edit_auth, const vector<Basetypes::account_auth>& auths)
		{
			const auto& auth = auths[0];

			XMAX_ASSERT(auths.size() == 1, authorization_exception,
				"should only have one declared authorization");
			XMAX_ASSERT(auth.account == acc_name, authorization_exception,
				"only can control self authorization.");

			if (auth_name == Config::xmax_owner_name)
			{
				XMAX_ASSERT(edit_auth == Config::xmax_owner_name, authorization_exception,
					"edit auth of 'ower auth' must be 'owner'.");
			}
			else if (auth_name == Config::xmax_active_name)
			{
				XMAX_ASSERT((edit_auth == Config::xmax_owner_name || edit_auth == Config::xmax_owner_name), authorization_exception,
					"modify auth of 'ower auth' must be 'owner' or 'active'.");
			}
			else
			{
				// need parent authority.
				const authority_object& authobj = get_authority_object(db, { auth_name, edit_auth });

			}
		}

		optional<authority_name>  find_linked_permission(const Basechain::database& db, account_name acc, account_name scope, func_name func)
		{
			auto key = std::make_tuple(acc, scope, func);
			const linked_permission_object* linked = db.find<linked_permission_object, by_func>(key);

			if (nullptr == linked)
			{
				// find again without func name.
				auto key2 = std::make_tuple(acc, scope, "");
				linked = db.find<linked_permission_object, by_func>(key2);
			}

			if (nullptr == linked)
			{
				return optional<authority_name>();
			}
			else
			{
				return linked->required_auth;
			}
		}

		optional<authority_name> min_linked_permission(const Basechain::database& db, account_name acc, account_name scope, func_name func)
		{
			try {
			optional<authority_name> linked = find_linked_permission(db, acc, scope, func);
			if (!linked)
			{
				return Config::xmax_active_name;
			}
			if (*linked == Config::xmax_sysany_name)
			{
				return optional<authority_name>();
			}
			return linked;
			} FC_CAPTURE_AND_RETHROW((acc)(scope)(func))
		}

		void check_updateauth(const Basechain::database& db, const vector<Basetypes::account_auth>& auths, const Basetypes::updateauth& args)
		{
			const auto& auth = auths[0];

			XMAX_ASSERT(auths.size() == 1, authorization_exception,
				"should only have one declared authorization");
			XMAX_ASSERT(auth.account == args.account, authorization_exception,
				"only can control self authorization.");

			// need parent authority.
			const authority_object* authobj = find_authority_object(db, { args.account, args.parent });

			XMAX_ASSERT(authobj != nullptr, authorization_exception,
				"error updateauth data, auth or parent of auth is not found.");

		}

		void check_deleteauth(const Basechain::database& db, const vector<Basetypes::account_auth>& auths, const Basetypes::deleteauth& args)
		{
			const auto& auth = auths[0];

			XMAX_ASSERT(auths.size() == 1, authorization_exception,
				"should only have one declared authorization");
			XMAX_ASSERT(auth.account == args.account, authorization_exception,
				"only can control self authorization.");
		}

		void check_linkauth(const Basechain::database& db, const vector<Basetypes::account_auth>& auths, const Basetypes::linkauth& args)
		{
			const auto& auth = auths[0];

			XMAX_ASSERT(auths.size() == 1, authorization_exception,
				"should only have one declared authorization");
			XMAX_ASSERT(auth.account == args.account, authorization_exception,
				"only can control self authorization.");
		}

		void check_unlinkauth(const Basechain::database& db, const vector<Basetypes::account_auth>& auths, const Basetypes::unlinkauth& args)
		{
			const auto& auth = auths[0];

			XMAX_ASSERT(auths.size() == 1, authorization_exception,
				"should only have one declared authorization");
			XMAX_ASSERT(auth.account == args.account, authorization_exception,
				"only can control self authorization.");

		}

		void check_authorization(const Basechain::database& db, const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys)
		{
			//return;
			for (const Basetypes::message& itr : messages)
			{
				const message_xmax& msg = message_xmax::cast(itr);
				bool special_flag = true;
				// check system auth.
				if (msg.code == Config::xmax_contract_name)
				{		
					if (updateauth_name == msg.type)
					{
						check_updateauth(db, msg.authorization, msg.as<Basetypes::updateauth>());
					}
					else if (deleteauth_name == msg.type)
					{
						check_deleteauth(db, msg.authorization, msg.as<Basetypes::deleteauth>());
					}
					else if (linkauth_name == msg.type)
					{
						check_linkauth(db, msg.authorization, msg.as<Basetypes::linkauth>());
					}
					else if (unlinkauth_name == msg.type)
					{
						check_unlinkauth(db, msg.authorization, msg.as<Basetypes::unlinkauth>());
					}
					else
					{
						special_flag = false;
					}
				}

				std::set<Basetypes::account_auth> satisfys;

				// check custom auth.
				for (const Basetypes::account_auth& au : msg.authorization)
				{
					if (!special_flag)
					{
						optional<authority_name> auth = min_linked_permission(db, au.account, msg.code, msg.type);

						aut
					}
				}
			}




		}

		void check_gaspayer(const Basechain::database& db, transaction_request_ptr transaction)
		{
			if (transaction->signed_trx.gaslimit==0)
			{
				return;
			}
			const auto& token_obj = db.get<xmx_token_object, by_owner_name>(transaction->signed_trx.gas_payer);
			XMAX_ASSERT(token_obj.main_token>=transaction->signed_trx.gaslimit, transaction_exception,
				"account must have more balance than gaslimit");
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
			XMAX_ASSERT(range.first == range.second, authorization_exception,
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