/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <chaindata_plugin.hpp>
#include <blockchain_plugin.hpp>
#include <genesis_state.hpp>
#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>

namespace Xmaxplatform {


class chain_plugin_impl {
public:
    Baseapp::bfs::path                   genesis_file;
    std::unique_ptr<Chain::chain_xmax>   chain;
};


    blockchain_plugin::blockchain_plugin()
:my(new chain_plugin_impl()) {
}

    blockchain_plugin::~blockchain_plugin(){}

void blockchain_plugin::set_program_options(options_description& cli, options_description& cfg)
{
    ilog("blockchain_plugin::set_program_options");
    cfg.add_options()
            ("genesis-json", bpo::value<boost::filesystem::path>(), "File to read Genesis State from");
}

void blockchain_plugin::plugin_initialize(const variables_map& options) {
    ilog("blockchain_plugin::plugin_initialize");
    if(options.count("genesis-json")) {
        my->genesis_file = options.at("genesis-json").as<Baseapp::bfs::path>();
    }
    auto genesis = fc::json::from_file(my->genesis_file).as<Native_contract::genesis_state_type>();
}

void blockchain_plugin::plugin_startup() {
    ilog("blockchain_plugin::plugin_startup");
    auto& data = app().get_plugin<chaindata_plugin>().data();
    my->chain.reset(new Chain::chain_xmax(data));
}

void blockchain_plugin::plugin_shutdown() {
    ilog("blockchain_plugin::plugin_startup");
}

    Chain::chain_xmax& blockchain_plugin::getchain() { return *my->chain; }
    const Chain::chain_xmax& blockchain_plugin::getchain() const { return *my->chain; }
} // namespace Xmaxplatform
