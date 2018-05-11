#pragma once
#include <xmaxlib/core.h>

extern "C" {
 
   uint32_t read_message( void* msg, uint32_t len );

   uint32_t message_size();

   void require_notice( account_name name );

   void require_auth( account_name name );

   account_name current_code();

}
