#pragma once
#include <xmaxlib/message.h>

namespace Xmaxplatform {

   template<typename T>
   T current_message() {
      T value;
      auto read = read_message( &value, sizeof(value) );
      xmax_assert( read >= sizeof(value), "message shorter than expected" );
      return value;
   }

   using ::require_auth;
   using ::require_notice;

   template<typename... Accounts>
   void require_notice( account_name name, Accounts... remaining_accounts ){
      require_notice( name );
      require_notice( remaining_accounts... );
   }


}
