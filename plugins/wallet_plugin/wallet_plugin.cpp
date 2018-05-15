/**
*  @file
*  @copyright defined in xmax/LICENSE.txt
*/
#include <blockchain_exceptions.hpp>
#include <wallet_plugin.hpp>
#include <chainhttp_plugin.hpp>

#include <blockchain_types.hpp>
#include <key_conversion.hpp>
#include <transaction.hpp>

#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/io/fstream.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/crypto/elliptic.hpp>

using namespace std;
using namespace Xmaxplatform;
using namespace Xmaxplatform::Chain;
using namespace Xmaxplatform::Utilities;
using namespace Xmaxplatform::Basetypes;

namespace Xmaxplatform {


	class wallet_plugin_impl {

	public:

		struct empty {};

		//-------------------------------------
		using create_key_params = empty;
		struct create_key_results {
			std::string public_key;
			std::string private_key;
		};
		create_key_results create_key(const create_key_params&) const;

		//-------------------------------------
		struct sign_transaction_params {
			Xmaxplatform::Chain::signed_transaction transaction;
			std::string private_key;
			Chain::chain_id_type chain_id;

		};

		struct sign_transaction_results {
			Xmaxplatform::Chain::signed_transaction transaction;
		};

		sign_transaction_results sign_transaction(const sign_transaction_params& params);
	};




#define CALL(api_name, api_handle, api_namespace, call_name, http_response_code) \
{std::string("/v0/" #api_name "/" #call_name), \
   [this](std::string, std::string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             auto result = api_handle->call_name(fc::json::from_string(body).as<api_namespace::call_name ## _params>()); \
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

#define WALLET_CALL(call_name, http_response_code) CALL(wallet, impl, Xmaxplatform::wallet_plugin_impl, call_name, http_response_code)

	/*!
	 * \class wallet_plugin_impl implementations
	 *
	 */

	 //--------------------------------------------------
	wallet_plugin_impl::create_key_results wallet_plugin_impl::create_key(const create_key_params&) const
	{
		create_key_results result;
		auto privateKey = fc::ecc::private_key::generate();
		std::string pubKey = std::string(public_key_type(privateKey.get_public_key()));
		result.private_key = key_to_wif(privateKey.get_secret());
		result.public_key = pubKey;
		return result;
	}


	//--------------------------------------------------
	wallet_plugin_impl::sign_transaction_results wallet_plugin_impl::sign_transaction(const sign_transaction_params& params)
	{
		sign_transaction_results result;
		Xmaxplatform::Chain::signed_transaction strans(params.transaction);

		fc::optional<fc::ecc::private_key> private_key = Utilities::wif_to_key(params.private_key);
		FC_ASSERT(private_key, "Invalid WIF-format private key ${key_string}",
			("key_string", params.private_key));

		strans.sign(*private_key, params.chain_id);

		result.transaction = strans;

		return result;
	}

	/*!
	* \class wallet_plugin implementations
	*
	*/

	wallet_plugin::wallet_plugin() {}
	wallet_plugin::~wallet_plugin() {}

	void wallet_plugin::set_program_options(options_description&, options_description& cfg) {
	}

	void wallet_plugin::plugin_initialize(const variables_map& options) {
	}

	void wallet_plugin::plugin_startup() {
		app().get_plugin<chainhttp_plugin>().add_api({
			WALLET_CALL(create_key, 200),
			WALLET_CALL(sign_transaction, 201)
			});
		impl.reset(new wallet_plugin_impl);
	}

	void wallet_plugin::plugin_shutdown() {

	}

}


FC_REFLECT(Xmaxplatform::wallet_plugin_impl::empty, )
FC_REFLECT(Xmaxplatform::wallet_plugin_impl::create_key_results, (public_key)(private_key))
FC_REFLECT(Xmaxplatform::wallet_plugin_impl::sign_transaction_params, (transaction)(private_key)(chain_id))
FC_REFLECT(Xmaxplatform::wallet_plugin_impl::sign_transaction_results, (transaction))