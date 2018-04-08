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
using namespace Baseapp;
namespace Xmaxplatform {



class blockbuilder_plugin : public plugin<blockbuilder_plugin> {
public:

    BASEAPP_DEPEND_PLUGINS((chaindata_plugin))
    blockbuilder_plugin();
   virtual ~blockbuilder_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
    std::unique_ptr<class chain_plugin_impl> my;
};

}
