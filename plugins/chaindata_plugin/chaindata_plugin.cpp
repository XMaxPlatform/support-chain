/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <chaindata_plugin.hpp>

#include <fc/filesystem.hpp>
#include <fc/log/logger.hpp>
namespace Xmaxplatform {

class chaindata_plugin_impl {
public:
    bool open_flag = false;
    uint64_t  shared_memory_size = 0;
    Basechain::bfs::path shared_memory_dir;

   fc::optional<Basechain::database> data;
};

    chaindata_plugin::chaindata_plugin() : my(new chaindata_plugin_impl){}
    chaindata_plugin::~chaindata_plugin(){}

void chaindata_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
           ("readonly", bpo::value<bool>()->default_value(false), "open data mode")
           ("shared-file-dir", bpo::value<Basechain::bfs::path>()->default_value("xmaxchain"),
            "the location of xmax chain shared memory files (absolute path or relative to application data dir)")
           ("shared-file-size", bpo::value<uint64_t>()->default_value(8*1024),
            "Minimum size MB of database shared memory file")
           ;
}

void chaindata_plugin::wipe_database() {
   if (!my->shared_memory_dir.empty() && !my->data.valid())
      fc::remove_all(my->shared_memory_dir);
   else
      elog("ERROR: database_plugin::wipe_database() called before configuration or after startup. Ignoring.");
}

void chaindata_plugin::plugin_initialize(const variables_map& options) {
   ilog("chaindata_plugin::plugin_initialize");
   my->shared_memory_dir = app().data_dir() / "blockchain";
   if(options.count("shared-file-dir")) {
      auto sfd = options.at("shared-file-dir").as<Basechain::bfs::path>();
      if(sfd.is_relative())
         my->shared_memory_dir = app().data_dir() / sfd;
      else
         my->shared_memory_dir = sfd;
   }
   my->shared_memory_size = options.at("shared-file-size").as<uint64_t>() * 1024 * 1024;
   my->open_flag = options.at("readonly").as<bool>();
}

void chaindata_plugin::plugin_startup() {
   ilog("chaindata_plugin::plugin_startup");
   using Basechain::database;
   my->data = Basechain::database(my->shared_memory_dir,
                                my->open_flag? database::read_only : database::read_write,
                                my->shared_memory_size);
}

void chaindata_plugin::plugin_shutdown() {
   ilog("closing database");
   my->data.reset();
   ilog("database closed successfully");
}

Basechain::database& chaindata_plugin::data() {
   assert(my->data.valid());
   return *my->data;
}

const Basechain::database& chaindata_plugin::data() const {
   assert(my->data.valid());
   return *my->data;
}

}
