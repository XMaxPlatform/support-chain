/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <blockchain_plugin.hpp>
#include <connection_xmax.hpp>
#include<protocol.hpp>
namespace Xmaxplatform {
   using namespace Baseapp;
   

   class chainnet_plugin : public Baseapp::plugin<chainnet_plugin>
   {
      public:
		  chainnet_plugin();
        virtual ~chainnet_plugin();

		BASEAPP_DEPEND_PLUGINS((blockchain_plugin))
        virtual void set_program_options(options_description& cli, options_description& cfg) override;

        void plugin_initialize(const variables_map& options);
        void plugin_startup();
        void plugin_shutdown();

        void   broadcast_block(const Chain::signed_block &sb);
		void   broadcast_confirm(const Chain::block_confirmation& confirm);

		Chain::string                       connect( const Chain::string& endpoint );
		Chain::string                       disconnect( const Chain::string& endpoint );
        optional<connection_status>  status( const Chain::string& endpoint )const;
		Chain::vector<connection_status>    connections()const;

        size_t num_peers() const;
      private:
        std::unique_ptr<class chainnet_plugin_impl> my;
   };

}


