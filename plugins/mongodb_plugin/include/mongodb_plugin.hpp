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

using mongodb_plugin_impl_ptr = std::shared_ptr<class mongodb_plugin_impl>;

/**
 * Provides persistence to MongoDB for:
 *   Blocks
 *   Transactions
 *   Messages
 *   Accounts
 *
 *   See data dictionary (DB Schema Definition - XMAX API) for description of MongoDB schema.
 *
 *   The goal ultimately is for all basechain data to be mirrored in MongoDB via a delayed node processing
 *   irreversible blocks. Currently, only Blocks, Transactions, Messages, and Account balance it mirrored.
 *   Basechain is being rewritten to be multi-threaded. Once basechain is stable, integration directly with
 *   a mirror database approach can be followed removing the need for the direct processing of Blocks employed
 *   with this implementation.
 *
 *   If MongoDB env not available (#ifndef MONGODB) this plugin is a no-op.
 */
class mongodb_plugin : public plugin<mongodb_plugin> {
public:
	
   mongodb_plugin();
   virtual ~mongodb_plugin();

   BASEAPP_DEPEND_PLUGINS()
   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   // This may only be called after plugin_initialize() and before plugin_startup()!
   void wipe_database();
   void applied_irreversible_block(const Chain::signed_block& block);

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
   mongodb_plugin_impl_ptr my;
};

}

