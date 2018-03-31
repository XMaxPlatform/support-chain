/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#include <blockchain_plugin.hpp>

#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>

namespace Xmaxplatform {


class chain_plugin_impl {
public:
   bfs::path                        genesis_file;
};


    blockchain_plugin::blockchain_plugin()
:my(new chain_plugin_impl()) {
}

    blockchain_plugin::~blockchain_plugin(){}

void blockchain_plugin::set_program_options(options_description& cli, options_description& cfg)
{

}

void blockchain_plugin::plugin_initialize(const variables_map& options) {
    ilog("blockchain_plugin::plugin_initialize");
}

void blockchain_plugin::plugin_startup() {
    ilog("blockchain_plugin::plugin_startup");
}

void blockchain_plugin::plugin_shutdown() {
    ilog("blockchain_plugin::plugin_startup");
}

} // namespace Xmaxplatform
