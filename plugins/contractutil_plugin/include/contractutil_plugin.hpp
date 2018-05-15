/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <plugin.hpp>

using namespace Baseapp;
namespace Xmaxplatform {
	class contractutil_plugin : public plugin<contractutil_plugin> {
	public:
		contractutil_plugin();
		~contractutil_plugin();

		BASEAPP_DEPEND_PLUGINS((chainhttp_plugin))
			virtual void set_program_options(options_description&, options_description& cfg) override;

		void plugin_initialize(const variables_map& options);
		void plugin_startup();
		void plugin_shutdown();

	private:
		std::unique_ptr<struct customised_plugin_impl> my;
	};

}

