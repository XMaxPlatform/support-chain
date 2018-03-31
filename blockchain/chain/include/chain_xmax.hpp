/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once


#include <basechain.hpp>
#include <fc/scoped_exit.hpp>

#include <boost/signals2/signal.hpp>

#include <fc/log/logger.hpp>

#include <map>

namespace Xmaxplatform { namespace Chain {
   using database = Basechain::database;

   class chain_xmax {
      public:

         chain_xmax(database& database);
         chain_xmax(const chain_xmax&) = delete;
         chain_xmax(chain_xmax&&) = delete;
         chain_xmax& operator=(const chain_xmax&) = delete;
         chain_xmax& operator=(chain_xmax&&) = delete;
         ~chain_xmax();

   private:
       database&                        _data;

       void setup_data_indexes();
       void initialize_chain();
   };

} }
