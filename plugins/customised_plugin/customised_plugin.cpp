/**
*  @file
*  @copyright defined in xmax/LICENSE.txt
*/
#include <customised_plugin.hpp>
#include <../../blockchain_plugin/include/blockchain_plugin.hpp>
#include<../../../foundation/utilities/include/key_conversion.hpp>
#include<../../../blockchain/chain/include/blockchain_types.hpp>

#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/variant.hpp>

#include <boost/asio/high_resolution_timer.hpp>
#include <boost/algorithm/clamp.hpp>

namespace Xmaxplatform {
	namespace detail {
		struct customised_plugin_empty {};
	}
}

FC_REFLECT(Xmaxplatform::detail::customised_plugin_empty, );

namespace Xmaxplatform {

	using namespace Xmaxplatform::Chain;
	using Chain::public_key_type;

#define CALL(api_name, api_handle, call_name, INVOKE, http_response_code) \
{std::string("/v0/" #api_name "/" #call_name), \
   [this](std::string, std::string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             INVOKE \
             cb(http_response_code, fc::json::to_string(result)); \
          } catch (fc::eof_exception& e) { \
             error_results results{400, "Bad Request", e.to_string()}; \
             cb(400, fc::json::to_string(results)); \
             elog("Unable to parse arguments: ${args}", ("args", body)); \
          } catch (fc::exception& e) { \
             error_results results{500, "Internal Service Error", e.to_detail_string()}; \
             cb(500, fc::json::to_string(results)); \
             elog("Exception encountered while processing ${call}: ${e}", ("call", #api_name "." #call_name)("e", e)); \
          } \
       }}

#define INVOKE_V_R_R(api_handle, call_name, in_param0, in_param1) \
     const auto& vs = fc::json::json::from_string(body).as<fc::variants>(); \
     api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>()); \
     Xmaxplatform::detail::customised_plugin_empty result;

#define INVOKE_V_R_R_R(api_handle, call_name, in_param0, in_param1,in_param2) \
     const auto& vs = fc::json::json::from_string(body).as<fc::variants>(); \
     api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(),vs.at(2).as<in_param2>()); \
     Xmaxplatform::detail::customised_plugin_empty result;

#define INVOKE_V_R_R_R_R(api_handle, call_name, in_param0, in_param1,in_param2,in_param3) \
     const auto& vs = fc::json::json::from_string(body).as<fc::variants>(); \
     api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(),vs.at(2).as<in_param2>(),vs.at(3).as<in_param3>()); \
     Xmaxplatform::detail::customised_plugin_empty result;

#define INVOKE_V_R_R_R_R_R(api_handle, call_name, in_param0, in_param1, in_param2, in_param3, in_param4) \
     const auto& vs = fc::json::json::from_string(body).as<fc::variants>(); \
     api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(), vs.at(2).as<in_param2>(), vs.at(3).as<in_param3>(), vs.at(4).as<in_param4>()); \
     Xmaxplatform::detail::customised_plugin_empty result;

#define INVOKE_V_V(api_handle, call_name) \
     api_handle->call_name(); \
     Xmaxplatform::detail::customised_plugin_empty result;

	static std::vector<name> sort_names(std::vector<name>&& names) {
		std::sort(names.begin(), names.end());
		auto itr = std::unique(names.begin(), names.end());
		names.erase(itr, names.end());
		return names;
	}

	struct customised_plugin_impl {

		void push_transaction(const std::string& callerPK, const std::string& callername, const fc::variant& vscope, const fc::variant& v)
		{
			Chain::chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);
			name creator(callername);
			Chain::signed_transaction trx;

#define GET_FIELD( VO, FIELD, RESULT ) \
       if( VO.contains(#FIELD) ) fc::from_variant( VO[#FIELD], RESULT.FIELD )
	   {
		   const variant_object& vo = v.get_object();


		   message result;
		   GET_FIELD(vo, code, result);
		   GET_FIELD(vo, type, result);
		   GET_FIELD(vo, authorization, result);

		   if (vo.contains("data")) {
			   const auto& data = vo["data"];

			   if (data.is_string()) {
				   // GET_FIELD( vo, data, result.messages[i] );
			   }
			   else if (data.is_object()) {
				   result.data = cc.message_to_binary(result.code, result.type, data);
			   }
			   trx.messages.push_back(result);

		   }
	   }
	   {

		   const std::vector<variant>& vlist = vscope.get_array();
		   Xmaxplatform::Basetypes::vector<Xmaxplatform::Basetypes::name> scopelist;
		   for (int i = 0; i<vlist.size(); i++)
		   {
			   std::string tempstrname;
			   fc::from_variant(vlist[i], tempstrname);
			   scopelist.push_back(Xmaxplatform::Basetypes::string_to_name(tempstrname.c_str()));
		   }

		   trx.scope = sort_names(std::move(scopelist));
	   }

	   trx.expiration = cc.head_block_time() + fc::seconds(30);
	   transaction_set_reference_block(trx, cc.head_block_id());

	   fc::ecc::private_key caller_priv_key = *Utilities::wif_to_key(callerPK);
	   trx.sign(caller_priv_key, chainid);

	   cc.push_transaction(trx);
		}

		void create_account(const std::string& init_name, const std::string& account_name, const std::string& init_priv_key, const public_key_type& pub_key_owner, const public_key_type& pub_key_active)
		{
			name accountName(account_name.c_str());
			name creator(init_name);

			chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);

			uint64_t stake = 10000;

			fc::ecc::private_key creator_priv_key = *Utilities::wif_to_key(init_priv_key);

			Chain::signed_transaction trx;
			auto memo = fc::variant(fc::time_point::now()).as_string() + " " + fc::variant(fc::time_point::now().time_since_epoch()).as_string();


			{
				auto owner_auth = Xmaxplatform::Chain::authority{ 1,{ { pub_key_owner, 1 } },{} };
				auto active_auth = Xmaxplatform::Chain::authority{ 1,{ { pub_key_active, 1 } },{} };
				auto recovery_auth = Xmaxplatform::Chain::authority{ 1,{},{ { { creator, "active" }, 1 } } };

				trx.scope = sort_names({ creator,Config::xmax_contract_name });
				transaction_emplace_message(trx, Config::xmax_contract_name, Xmaxplatform::Basetypes::vector<Basetypes::account_permission>{ {creator, "active"}}, "newaccount",
					Basetypes::newaccount{ creator, accountName, owner_auth,
					active_auth, recovery_auth, stake });
			}

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());
			trx.sign(creator_priv_key, chainid);

			cc.push_transaction(trx);
		}

		void create_test_accounts(const std::string& init_name, const std::string& init_priv_key) {
			name newaccountA("txn.test.a");
			name newaccountB("txn.test.b");
			name creator(init_name);

			chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);
			uint64_t stake = 10000;

			fc::ecc::private_key txn_test_receiver_A_priv_key = fc::ecc::private_key::regenerate(fc::sha256(std::string(64, 'a')));
			fc::ecc::private_key txn_test_receiver_B_priv_key = fc::ecc::private_key::regenerate(fc::sha256(std::string(64, 'b')));
			fc::ecc::public_key  txn_text_receiver_A_pub_key = txn_test_receiver_A_priv_key.get_public_key();
			fc::ecc::public_key  txn_text_receiver_B_pub_key = txn_test_receiver_B_priv_key.get_public_key();
			fc::ecc::private_key creator_priv_key = *Utilities::wif_to_key(init_priv_key);

			Chain::signed_transaction trx;
			auto memo = fc::variant(fc::time_point::now()).as_string() + " " + fc::variant(fc::time_point::now().time_since_epoch()).as_string();

			//create "A" account
			{
				auto owner_auth = Xmaxplatform::Chain::authority{ 1,{ { txn_text_receiver_A_pub_key, 1 } },{} };
				auto active_auth = Xmaxplatform::Chain::authority{ 1,{ { txn_text_receiver_A_pub_key, 1 } },{} };
				auto recovery_auth = Xmaxplatform::Chain::authority{ 1,{},{ { { creator, "active" }, 1 } } };

				trx.scope = sort_names({ creator,Config::xmax_contract_name });
				transaction_emplace_message(trx, Config::xmax_contract_name,Xmaxplatform::Basetypes::vector<Basetypes::account_permission>{ {creator, "active"}}, "newaccount",
					Basetypes::newaccount{ creator, newaccountA, owner_auth,
					active_auth, recovery_auth, stake });
			}
			//create "B" account
			{
				auto owner_auth = Xmaxplatform::Chain::authority{ 1,{ { txn_text_receiver_B_pub_key, 1 } },{} };
				auto active_auth = Xmaxplatform::Chain::authority{ 1,{ { txn_text_receiver_B_pub_key, 1 } },{} };
				auto recovery_auth = Xmaxplatform::Chain::authority{ 1,{},{ { { creator, "active" }, 1 } } };

				trx.scope = sort_names({ creator,Config::xmax_contract_name });
				transaction_emplace_message(trx, Config::xmax_contract_name, Xmaxplatform::Basetypes::vector<Basetypes::account_permission>{ {creator, "active"}}, "newaccount",
					Basetypes::newaccount{ creator, newaccountB, owner_auth,
					active_auth, recovery_auth, stake });
			}

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());
			trx.sign(creator_priv_key, chainid);

			cc.push_transaction(trx);
		}

		void fund_accounts(const std::string& fund_name, const std::string& fund_priv_key) {
			name newaccountA("txn.test.a");
			name newaccountB("txn.test.b");
			name fundor(fund_name);

			fc::ecc::private_key fundor_priv_key = *Utilities::wif_to_key(fund_priv_key);

			chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);
			uint64_t balance = 10000;

			auto memo = fc::variant(fc::time_point::now()).as_string() + " " + fc::variant(fc::time_point::now().time_since_epoch()).as_string();

			Chain::signed_transaction trx;
			trx.scope = sort_names({ fundor,newaccountA,newaccountB });
			transaction_emplace_message(trx, Config::xmax_contract_name,
				Basetypes::vector<Basetypes::account_permission>{ {fundor, "active"}},
				"transfer", Basetypes::transfer{ fundor, newaccountA, balance, memo });
			transaction_emplace_message(trx, Config::xmax_contract_name,
				Basetypes::vector<Basetypes::account_permission>{ {fundor, "active"}},
				"transfer", Basetypes::transfer{ fundor, newaccountB, balance, memo });
			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());
			trx.sign(fundor_priv_key, chainid);

			cc.push_transaction(trx);
		}

		void start_generation(const std::string& salt, const uint64_t& persecond) {
			if (running)
				throw fc::exception(fc::invalid_operation_exception_code);
			running = true;
			memo_salt = salt;

			timer_timeout = 1000 / boost::algorithm::clamp(persecond / 2, 1, 200);

			ilog("Started transaction test plugin; performing ${p} transactions/second", ("p", 1000 / timer_timeout * 2));

			arm_timer(boost::asio::high_resolution_timer::clock_type::now());
		}

		void arm_timer(boost::asio::high_resolution_timer::time_point s) {
			timer.expires_at(s + std::chrono::milliseconds(timer_timeout));
			timer.async_wait([this](auto ec) {
				if (ec)
					return;
				try {
					this->send_transaction();
				}
				catch (fc::exception e) {
					elog("pushing transaction failed: ${e}", ("e", e.to_detail_string()));
					this->stop_generation();
					return;
				}
				this->arm_timer(timer.expires_at());
			});
		}

		void send_transaction() {
			Chain::signed_transaction trx;

			chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);

			name sender("txn.test.a");
			name recipient("txn.test.b");

			//make transaction a->b
			std::string memo = memo_salt + fc::variant(fc::time_point::now()).as_string() + " " + fc::variant(fc::time_point::now().time_since_epoch()).as_string();
			trx.scope = sort_names({ sender,recipient });
			transaction_emplace_message(trx, Config::xmax_contract_name,
				Basetypes::vector<Basetypes::account_permission>{ {sender, "active"}},
				"transfer", Basetypes::transfer{ sender, recipient, 1, memo });

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());

			fc::ecc::private_key creator_priv_key = fc::ecc::private_key::regenerate(fc::sha256(std::string(64, 'a')));
			trx.sign(creator_priv_key, chainid);
			cc.push_transaction(trx);

			//make transaction b->a
			trx.clear();
			trx.scope = sort_names({ sender,recipient });
			transaction_emplace_message(trx, Config::xmax_contract_name,
				Basetypes::vector<Basetypes::account_permission>{ {recipient, "active"}},
				"transfer", Basetypes::transfer{ recipient, sender, 1, memo });

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());

			fc::ecc::private_key b_priv_key = fc::ecc::private_key::regenerate(fc::sha256(std::string(64, 'b')));
			trx.sign(b_priv_key, chainid);
			cc.push_transaction(trx);
		}

		void stop_generation() {
			if (!running)
				throw fc::exception(fc::invalid_operation_exception_code);
			timer.cancel();
			running = false;
			ilog("Stopping transaction generation test");
		}

		boost::asio::high_resolution_timer timer{ app().get_io_service() };
		bool running{ false };

		unsigned timer_timeout;

		std::string memo_salt;
	};

	customised_plugin::customised_plugin() {}
	customised_plugin::~customised_plugin() {}

	void customised_plugin::set_program_options(options_description&, options_description& cfg) {
	}

	void customised_plugin::plugin_initialize(const variables_map& options) {
	}

	void customised_plugin::plugin_startup() {
		app().get_plugin<chainhttp_plugin>().add_api({
			CALL(customised_plugin, my, create_test_accounts, INVOKE_V_R_R(my, create_test_accounts, std::string, std::string), 200),
			CALL(customised_plugin, my, fund_accounts, INVOKE_V_R_R(my, fund_accounts, std::string, std::string), 200),
			CALL(customised_plugin, my, stop_generation, INVOKE_V_V(my, stop_generation), 200),
			CALL(customised_plugin, my, start_generation, INVOKE_V_R_R(my, start_generation, std::string, uint64_t), 200),
			CALL(customised_plugin, my, create_account, INVOKE_V_R_R_R_R_R(my, create_account, std::string, std::string, std::string, public_key_type, public_key_type), 200),
			CALL(customised_plugin, my, push_transaction, INVOKE_V_R_R_R_R(my, push_transaction, std::string, std::string, fc::variant , fc::variant), 200)
		});
		my.reset(new customised_plugin_impl);
	}

	void customised_plugin::plugin_shutdown() {
		try {
			my->stop_generation();
		}
		catch (fc::exception e) {
		}
	}

}
