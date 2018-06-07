/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <application.hpp>
#include <plugin.hpp>
#include <block.hpp>

using namespace Baseapp;

namespace Xmaxplatform {

using mongodb_plugin_impl_ptr = std::unique_ptr<class mongodb_plugin_impl>;

/**
 * MongoDB for blockchain
 */
class mongodb_plugin : public plugin<mongodb_plugin> {
public:
	
   mongodb_plugin();
   virtual ~mongodb_plugin();

   BASEAPP_DEPEND_PLUGINS()
   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();
  
   void drop_tables_on_init();
   void finalize_block(const Chain::signed_block& block);

private:
	void _connect_mongo_db(const std::string& mongo_uri_str);


private:
   mongodb_plugin_impl_ptr db_impl;
};

}

