/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <blockchain_plugin.hpp>
#include <genesis_state.hpp>
#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>
#include <blockchain_exceptions.hpp>

#include <native_contract_chain_init.hpp>

namespace Xmaxplatform {


    class chain_plugin_impl {
    public:
        Baseapp::bfs::path genesis_file;
        std::unique_ptr<Chain::chain_xmax> chain;
    };


    blockchain_plugin::blockchain_plugin()
            : my(new chain_plugin_impl()) {
    }

    blockchain_plugin::~blockchain_plugin() {}

    void blockchain_plugin::set_program_options(options_description &cli, options_description &cfg) {
        ilog("blockchain_plugin::set_program_options");
        cfg.add_options()
                ("genesis-json", bpo::value<boost::filesystem::path>(), "File to read Genesis State from");
    }

    void blockchain_plugin::plugin_initialize(const variables_map &options) {
        ilog("blockchain_plugin::plugin_initialize");
        if (options.count("genesis-json")) {
            my->genesis_file = options.at("genesis-json").as<Baseapp::bfs::path>();
        }

    }

#define CALL(api_name, api_handle, api_namespace, call_name, http_response_code) \
{std::string("/v0/" #api_name "/" #call_name), \
   [this, api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             auto result = api_handle.call_name(fc::json::from_string(body).as<api_namespace::call_name ## _params>()); \
             cb(http_response_code, fc::json::to_string(result)); \
          } catch (Chain::tx_missing_sigs& e) { \
             error_results results{401, "UnAuthorized", e.to_string()}; \
             cb(401, fc::json::to_string(results)); \
          } catch (Chain::tx_duplicate& e) { \
             error_results results{409, "Conflict", e.to_string()}; \
             cb(409, fc::json::to_string(results)); \
          } catch (Chain::transaction_exception& e) { \
             error_results results{400, "Bad Request", e.to_string()}; \
             cb(400, fc::json::to_string(results)); \
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

#define CHAIN_RO_CALL(call_name, http_response_code) CALL(xmaxchain, ro_api, Chain_APIs::read_only, call_name, http_response_code)

    void blockchain_plugin::plugin_startup() {
        ilog("blockchain_plugin::plugin_startup");
        auto &data = app().get_plugin<chaindata_plugin>().data();
        auto genesis = fc::json::from_file(my->genesis_file).as<Native_contract::genesis_state_type>();
        Native_contract::native_contract_chain_init chainsetup(genesis);
        my->chain.reset(new Chain::chain_xmax(data, chainsetup));

        register_chain_api();

    }

    void blockchain_plugin::register_chain_api() {
        ilog("register chain api");

        auto ro_api = get_read_only_api();

        app().get_plugin<chainhttp_plugin>().add_api({
                                                        CHAIN_RO_CALL(get_account, 200)

                                                });
    }

    void blockchain_plugin::plugin_shutdown() {
        ilog("blockchain_plugin::plugin_startup");
    }

    Chain::chain_xmax &blockchain_plugin::getchain() { return *my->chain; }

    const Chain::chain_xmax &blockchain_plugin::getchain() const { return *my->chain; }

namespace Chain_APIs{
    read_only::get_account_results read_only::get_account(const get_account_params &params) const {
        using namespace Xmaxplatform::Chain;

        get_account_results result;
        result.account_name = params.account_name;

        const auto &data = _chain.get_data();
        const auto &token = data.get<xmx_token_object, by_owner_name>(params.account_name);

        result.xmx_token = asset(token.xmx_token, XMX_SYMBOL);

        return result;
    }
}// namespace Chain_APIs
} // namespace Xmaxplatform
