/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <plugin.hpp>
#include <basechain.hpp>
using namespace Baseapp;
using namespace Basechain;
namespace Xmaxplatform {

class chaindata_plugin : public plugin<chaindata_plugin> {
public:
    chaindata_plugin();
   virtual ~chaindata_plugin();

   BASEAPP_DEPEND_PLUGINS()
   virtual void set_program_options(options_description&, options_description& cfg) override;

   // This may only be called after plugin_initialize() and before plugin_startup()!
   void wipe_database();

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();


   // This may only be called after plugin_startup()!
   database& data();
   // This may only be called after plugin_startup()!
   const database& data() const;

private:
   std::unique_ptr<class chaindata_plugin_impl> my;
};

}
