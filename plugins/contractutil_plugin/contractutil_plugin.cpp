/**
*  @file
*  @copyright defined in xmax/LICENSE.txt
*/
#include <blockchain_exceptions.hpp>
#include <contractutil_plugin.hpp>
#include <../../blockchain_plugin/include/blockchain_plugin.hpp>
#include <../../../foundation/utilities/include/key_conversion.hpp>
#include <../../../blockchain/chain/include/blockchain_types.hpp>

#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/io/fstream.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/variant.hpp>

#include <boost/asio/high_resolution_timer.hpp>
#include <boost/algorithm/clamp.hpp>

#include <WAST/WAST.h>
#include <Inline/Serialization.h>
#include <IR/Module.h>

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
	FC_DECLARE_EXCEPTION(explained_exception, 9000000, "explained exception, see error log");

	Basetypes::vector<uint8_t> assemble_wast(const std::string& wast) {
		IR::Module module;
		std::vector<WAST::Error> parseErrors;
		WAST::parseModule(wast.c_str(), wast.size(), module, parseErrors);
		if (parseErrors.size())
		{
			std::cerr << "Error parsing WebAssembly text file:" << std::endl;
			for (auto& error : parseErrors)
			{
				std::cerr << ":" << error.locus.describe() << ": " << error.message.c_str() << std::endl;
				std::cerr << error.locus.sourceLine << std::endl;
				std::cerr << std::setw(error.locus.column(8)) << "^" << std::endl;
			}
			FC_THROW_EXCEPTION(explained_exception, "wast parse error");
		}

		try
		{
			// Serialize the WebAssembly module.
			Serialization::ArrayOutputStream stream;
			WASM::serialize(stream, module);
			return stream.getBytes();
		}
		catch (Serialization::FatalSerializationException exception)
		{
			std::cerr << "Error serializing WebAssembly binary file:"<< std::endl;
			std::cerr << exception.message << std::endl;
			FC_THROW_EXCEPTION(explained_exception, "wasm serialize error");
		}
	}

	struct customised_plugin_impl {

		void set_code(const std::string& callername, const std::string& wastPath, const std::string& abiPath)
		{
			std::string wast;
			std::cout << "Reading WAST..." << std::endl;
			fc::read_file_contents(wastPath, wast);
			std::cout << "Assembling WASM..." << std::endl;
			auto wasm = assemble_wast(wast);

			Basetypes::setcode handler;
			handler.account = callername;
			handler.code.assign(wasm.begin(), wasm.end());
			
			handler.code_abi = fc::json::from_file(abiPath).as<Basetypes::abi>();

			Chain::signed_transaction trx;
			trx.scope = sort_names({ Config::xmax_contract_name, callername });
			transaction_emplace_message(trx, Config::xmax_contract_name, Basetypes::vector<Basetypes::account_auth>{ {callername, Config::xmax_active_name}},
				"setcode", handler);

			std::cout << "Publishing contract..."<< std::endl;
			Chain::chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());
			cc.push_transaction(trx);
		
		}
#ifdef USE_V8
		void set_jscode(const std::string& callername, const std::string& wastPath, const std::string& abiPath)
		{
			std::string wast;
			std::cout << "Reading JS..." << std::endl;
			fc::read_file_contents(wastPath, wast);

			Basetypes::setcode handler;
			handler.account = callername;
			handler.code.assign(wast.begin(), wast.end());

			handler.code_abi = fc::json::from_file(abiPath).as<Basetypes::abi>();

			Chain::signed_transaction trx;
			trx.scope = sort_names({ Config::xmax_contract_name, callername });
			transaction_emplace_message(trx, Config::xmax_contract_name, Basetypes::vector<Basetypes::account_auth>{ {callername, Config::xmax_active_name}},
				"setjscode", handler);

			std::cout << "Publishing contract..." << std::endl;
			Chain::chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());
			cc.push_transaction(trx);

		}
#endif

		void issue_erc20(const std::string& callerPK,const std::string& callername, const std::string& erc20Name, const int& maxCount)
		{
			Chain::chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);

			Basetypes::adderc20 handler;
			handler.creator = Xmaxplatform::Basetypes::name::to_name_code(callername.c_str());
			handler.total_balance = maxCount;
			handler.token_name = token_name_from_string(erc20Name.c_str());

			Chain::signed_transaction trx;
			trx.scope = sort_names({ callername });
			transaction_emplace_message(trx, 
				Config::xmax_contract_name, 
				Basetypes::vector<Basetypes::account_auth>{ {callername, Config::xmax_active_name}},
				"adderc20", handler);

			std::cout << "adderc20" << std::endl;

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());

			fc::ecc::private_key caller_priv_key = *Utilities::wif_to_key(callerPK);
			trx.sign(caller_priv_key, chainid);
			cc.push_transaction(trx);

		}

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

				trx.scope = sort_names({ creator,Config::xmax_contract_name });
				transaction_emplace_message(trx, Config::xmax_contract_name, Xmaxplatform::Basetypes::vector<Basetypes::account_auth>{ {creator, Config::xmax_active_name}}, "addaccount",
					Basetypes::addaccount{ creator, accountName, owner_auth,
					active_auth, stake });
			}

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			transaction_set_reference_block(trx, cc.head_block_id());
			trx.sign(creator_priv_key, chainid);

			cc.push_transaction(trx);
		}
		void fix_push_trx(const fc::variant& var_params, const fc::variant& var_trx)
		{
			// parse transaction.
			blockchain_plugin& chain_plugin = app().get_plugin<blockchain_plugin>();

			Chain::signed_transaction trx;

			chain_plugin.getchain().parse_transaction(trx, var_trx);

			// signed transaction with private key.

			const variant_object& po = var_params.get_object();

			std::vector<std::string> callerPKs;
			fc::microseconds duration_sec = fc::seconds(60);


#define GET_FIELD( VO, FIELD, RESULT ) if( VO.contains(FIELD) ) fc::from_variant( VO[FIELD], RESULT )

			GET_FIELD(po, "sign_keys", callerPKs);
			GET_FIELD(po, "duration_sec", duration_sec);

#undef GET_FIELD 

			if (Basetypes::time(fc::microseconds(0)) == trx.expiration)
			{
				trx.expiration = chain_plugin.getchain().head_block_time() + duration_sec;
			}
			if (0 == trx.ref_block_num || 0 == trx.ref_block_prefix)
			{
				transaction_set_reference_block(trx, chain_plugin.getchain().head_block_id());
			}

			Chain::chain_id_type chainid;
			app().get_plugin<blockchain_plugin>().get_chain_id(chainid);

			for (const auto& str : callerPKs)
			{
				fc::optional<fc::ecc::private_key> caller_priv_key = Utilities::wif_to_key(str);
				if (caller_priv_key.valid())
				{
					trx.sign(*caller_priv_key, chainid);
				}
			}

			Chain::transaction_package_ptr new_package = std::make_shared<Chain::transaction_package>(trx);

			// push transaction to chain.
			auto result = chain_plugin.get_read_write_api().push_transaction_package(new_package);
	
		}
	};

	contractutil_plugin::contractutil_plugin() {}
	contractutil_plugin::~contractutil_plugin() {}

	void contractutil_plugin::set_program_options(options_description&, options_description& cfg) {
	}

	void contractutil_plugin::plugin_initialize(const variables_map& options) {
	}

	void contractutil_plugin::plugin_startup() {
		app().get_plugin<chainhttp_plugin>().add_api({
			CALL(contractutil_plugin, my, create_account, INVOKE_V_R_R_R_R_R(my, create_account, std::string, std::string, std::string, public_key_type, public_key_type), 200),
			CALL(contractutil_plugin, my, push_transaction, INVOKE_V_R_R_R_R(my, push_transaction, std::string, std::string, fc::variant , fc::variant), 200),
			CALL(contractutil_plugin, my, set_code, INVOKE_V_R_R_R(my, set_code, std::string, std::string,  std::string), 200),
			CALL(contractutil_plugin, my, fix_push_trx, INVOKE_V_R_R(my, fix_push_trx, fc::variant, fc::variant), 200),
			CALL(contractutil_plugin, my, issue_erc20, INVOKE_V_R_R_R_R(my, issue_erc20, std::string, std::string, std::string,int), 200),

			
#ifdef USE_V8
			CALL(contractutil_plugin, my, set_jscode, INVOKE_V_R_R_R(my, set_jscode, std::string, std::string,  std::string), 200),
#endif
			
		});
		my.reset(new customised_plugin_impl);
	}

	void contractutil_plugin::plugin_shutdown() {
	
	}

}
