
#pragma once
#include <xmaxlib/transaction.h>
#include <xmaxlib/core.h>

namespace Xmaxplatform {
   class message {
   public:
      template<typename Payload, typename ...Permissions>
      message(const account_name& code, const func_name& type, const Payload& payload, Permissions... permissions )
         : handle(message_create(code, type, &payload, sizeof(Payload)))
      {
         add_permissions(permissions...);
      }

      template<typename Payload>
      message(const account_name& code, const func_name& type, const Payload& payload )
         : handle(message_create(code, type, &payload, sizeof(Payload)))
      {
      }

      message(const account_name& code, const func_name& type)
         : handle(message_create(code, type, nullptr, 0))
      {
      }

      // no copy constructor due to opaque handle
      message( const message& ) = delete;

      message( message&& msg ) {
         handle = msg.handle;
         msg.handle = invalid_message_handle;
      }

      ~message() {
         if (handle != invalid_message_handle) {
            message_drop(handle);
            handle = invalid_message_handle;
         }
      }

      void add_permissions(account_name account, permission_name permission) {
         message_require_permission(handle, account, permission);
      }

      template<typename ...Permissions>
      void add_permissions(account_name account, permission_name permission, Permissions... permissions) {
         add_permissions(account, permission);
         add_permissions(permissions...);
      }

      void send() {
         assert_valid_handle();
         message_send(handle);
         handle = invalid_message_handle;
      }

   private:
      void assert_valid_handle() {
         xmax_assert(handle != invalid_message_handle, "attempting to send or modify a finalized message" );
      }

      message_handle handle;

      friend class transaction;

   };
} 