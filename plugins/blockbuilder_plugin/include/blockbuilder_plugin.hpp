/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <plugin.hpp>
#include <boost/container/flat_set.hpp>
#include <chain_xmax.hpp>
using namespace Baseapp;
namespace Xmaxplatform {

    enum block_build_condition
    {
        generated = 0,
        exception = 1,
		not_time_yet,
		not_my_turn,
		no_private_key,
    };

class blockbuilder_plugin : public plugin<blockbuilder_plugin> {
public:

    BASEAPP_DEPEND_PLUGINS((blockchain_plugin))
    blockbuilder_plugin();
   virtual ~blockbuilder_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

   bool import_key(const account_name& builder, const Basetypes::string& private_key);

   void on_recv_message(const Chain::signed_block &msg);
   void on_recv_message(const Chain::block_confirmation& msg);

   Chain::vector<Chain::signed_block> get_sync_blocklist(const uint32_t& lastnum);

public:

private:
    std::unique_ptr<class blockbuilder_plugin_impl> my;
};

}
