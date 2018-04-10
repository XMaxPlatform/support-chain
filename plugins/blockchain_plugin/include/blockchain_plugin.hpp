/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <plugin.hpp>
#include <boost/container/flat_set.hpp>
#include <chain_xmax.hpp>
#include <chaindata_plugin.hpp>
#include <chainhttp_plugin.hpp>

using namespace Baseapp;
namespace Xmaxplatform {

    using namespace Basetypes;


    namespace Chain_APIs {
        struct empty{};

        class read_only {
            const Chain::chain_xmax& _chain;

        public:
            static const string KEYi64;
            static const string KEYstr;
            static const string KEYi128i128;
            static const string KEYi64i64i64;
            static const string PRIMARY;
            static const string SECONDARY;
            static const string TERTIARY;

            read_only(const Chain::chain_xmax& chain)
                    : _chain(chain) {}


            struct get_account_results {
                name                       account_name;
                asset                      xmx_token = asset(0,XMX_SYMBOL);
            };
            struct get_account_params {
                Basetypes::name account_name;
            };
            get_account_results get_account( const get_account_params& params )const;

        };

    } // namespace Chain_APIs


class blockchain_plugin : public plugin<blockchain_plugin> {
public:

    BASEAPP_DEPEND_PLUGINS((chaindata_plugin)(chainhttp_plugin))
    blockchain_plugin();
   virtual ~blockchain_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

    Chain::chain_xmax& getchain();
    const Chain::chain_xmax& getchain() const;

    Chain_APIs::read_only get_read_only_api() const { return Chain_APIs::read_only(getchain()); }
private:
    std::unique_ptr<class chain_plugin_impl> my;
    void register_chain_api();
};

}

FC_REFLECT( Xmaxplatform::Chain_APIs::read_only::get_account_results, (account_name)(xmx_token) )

FC_REFLECT( Xmaxplatform::Chain_APIs::read_only::get_account_params, (account_name) )