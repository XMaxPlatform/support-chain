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

		void noop_checktime() {}

		static std::function<void()> _noop_checktime{ &noop_checktime };
		static const int max_check_depth = 3;
		typedef std::set<Basetypes::account_auth> satisfy_dic;
		typedef flat_set<public_key_type> public_key_dic;

		struct auth_checker
		{
			vector<bool>  key_used;
			flat_set< std::pair<account_name, authority_name> > aucache;
			const satisfy_dic& sdic;
			const public_key_dic& pdic;
			const Basechain::database& db;

			auth_checker(const Basechain::database& _db, const satisfy_dic& _sdic, const public_key_dic& _pdic)
				: db(_db)
				, sdic(_sdic)
				, pdic(_pdic)
				, key_used(_pdic.size(), false)
			{

			}

			void check(const Basetypes::account_auth& accauth)
			{
				check_with_depth(accauth, 0);
			}

		private:

			void check_with_depth(const Basetypes::account_auth& accauth, int depth)
			{
				XMAX_ASSERT(depth < max_check_depth, authorization_exception,
					"depth in check must be less than ${depth}", ("depth", max_check_depth));

				check_impl(accauth, depth);
			}

			void check_impl(const Basetypes::account_auth& accauth, int depth)
			{
				const authority_object& authobj = get_authority_object(db, accauth);
				int current_weight = 0;
				int threshold = int(authobj.authoritys.threshold);

				// check key permissions.
				for (const key_permission& wkey : authobj.authoritys.keys)
				{
					auto itr = pdic.find(wkey.key);
					if (itr != pdic.end())
					{
						current_weight += int(wkey.weight);
						key_used[itr - pdic.begin()] = true;
					}
				}
				int newdepth = depth + 1;
				// check account permissions.
				for (const account_permission& wacc : authobj.authoritys.accounts)
				{
					if (aucache.find({ wacc.auth.account, wacc.auth.authority }) == aucache.end())
					{
						check_with_depth({ wacc.auth.account, wacc.auth.authority }, newdepth);
						aucache.insert({ wacc.auth.account, wacc.auth.authority });
					}
					current_weight += int(wacc.weight);
				}
				XMAX_ASSERT(current_weight >= threshold, authorization_exception,
					"no enough auth for ${auth}", ("auth", accauth));
			}

		};



		void check_authorization(const Basechain::database& db, const std::vector<Basetypes::message>& messages, const flat_set<public_key_type>& keys, const std::function<void()>&  checktime)
		{
			//return;
			const auto& checktimefunc = (static_cast<bool>(checktime) ? checktime : _noop_checktime);

			satisfy_dic satisfys;

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

				// check custom auth.
				for (const Basetypes::account_auth& au : msg.authorization)
				{
					checktimefunc();
					if (!special_flag)
					{
						optional<authority_name> permission = min_linked_permission(db, au.account, msg.code, msg.type);

						const authority_object& origin_auth = get_authority_object(db, au);
						const authority_object& sub_auth = get_authority_object(db, { au.account, *permission });

						XMAX_ASSERT(parent_permission_valid(db, origin_auth, sub_auth), transaction_exception, "error authority");
					}
					auto res = satisfys.emplace(au);
				}
			}

			// check all satisfys.

			auth_checker checker(db, satisfys, keys);

			for (const Basetypes::account_auth& accauth : satisfys)
			{
				const authority_object& authobj = get_authority_object(db, accauth);
				checker.check(accauth);
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

				auto key = std::make_tuple(auth.account, auth.authority);
				return db.get<authority_object, by_owner>(key);

			} FC_CAPTURE_AND_RETHROW(("auth", auth))
		}

		const authority_object* find_authority_object(const Basechain::database& db, const Basetypes::account_auth& auth)
		{
			try {
				FC_ASSERT(!auth.account.empty() && !auth.authority.empty(), "Invalid authority");

				auto key = std::make_tuple(auth.account, auth.authority);
				return db.find<authority_object, by_owner>(key);

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