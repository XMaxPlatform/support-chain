/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <plugin.hpp>
#include <boost/container/flat_set.hpp>
using namespace Baseapp;
namespace Xmaxplatform {



class blockchain_plugin : public plugin<blockchain_plugin> {
public:

    BASEAPP_DEPEND_PLUGINS()
    blockchain_plugin();
   virtual ~blockchain_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
    std::unique_ptr<class chain_plugin_impl> my;
};

}
