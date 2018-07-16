#pragma once
#include <xmaxlib/types.h>

extern "C" {
   
   typedef uint32_t message_handle;
   #define invalid_message_handle (0xFFFFFFFFUL)
   

   message_handle message_create(account_name code, func_name type, void const* data, int size);

 
   void message_require_permission(message_handle msg, account_name account, authority_name permission);


   void message_send(message_handle msg);

   
   void message_drop(message_handle msg);

}
