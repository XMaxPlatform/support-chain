/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <application.hpp>

#include <fc/log/logger_config.hpp>
#include <fc/log/appender.hpp>
#include <fc/exception/exception.hpp>
#include <fc/filesystem.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <chainhttp_plugin.hpp>
#include <wallet_plugin.hpp>

using namespace Baseapp;

namespace fc {
    std::unordered_map<std::string,appender::ptr>& get_appender_map();
}

void initialize_logging()
{
    auto config_path = app().get_logging_conf();
    if(fc::exists(config_path))
        fc::configure_logging(config_path);
    for(auto iter : fc::get_appender_map())
        iter.second->initialize(app().get_io_service());
}

void regist_plugins()
{    
    app().register_plugin<Xmaxplatform::chainhttp_plugin>();	
	app().register_plugin<Xmaxplatform::wallet_plugin>();	
}

int main(int argc, char** argv)
{
	regist_plugins();

	app().set_default_config_dir("wallet-config-dir");
	app().set_default_data_dir("wallet-data-dir");

	if (!app().init<Xmaxplatform::chainhttp_plugin>(argc, argv))
		return -1;

	initialize_logging();
	ilog("xmaxwallet init success!");
	app().startup();
	app().exec();

	return 0;
}
