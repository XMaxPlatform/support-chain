/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <xmaxtypes.hpp>

namespace Xmaxplatform { namespace Chain {


struct message_xmax : public Basetypes::message {
   message_xmax() = default;
   template<typename T>
   message_xmax(const Basetypes::account_name& code, const Basetypes::vector<Basetypes::account_auth>& authorization, const Basetypes::func_name& type, T&& value)
      :Basetypes::message(code, type, authorization, Basetypes::bytes()) {
      set<T>(type, std::forward<T>(value));
   }

   message_xmax(const Basetypes::account_name& code, const Basetypes::vector<Basetypes::account_auth>& authorization, const Basetypes::func_name& type)
      :Basetypes::message(code, type, authorization, Basetypes::bytes()) {}

   message_xmax(const Basetypes::message& m) : Basetypes::message(m) {}


   inline static message_xmax& cast(Basetypes::message& msg)
   {
	   return static_cast<message_xmax&>(msg);
   }

   inline static const message_xmax& cast(const Basetypes::message& msg)
   {
	   return static_cast<const message_xmax&>(msg);
   }

   template<typename T>
   void set_packed(const Basetypes::func_name& t, const T& value) {
      type = t;
      data.resize(sizeof(value));
      memcpy( data.data(), &value, sizeof(value) );
   }

   template<typename T>
   void set(const Basetypes::func_name& t, const T& value) {
      type = t;
      data = fc::raw::pack(value);
   }
   template<typename T>
   T as()const {
      return fc::raw::unpack<T>(data);
   }
};


struct message_receipt
{
	account_name			to_code; // account name of contract code.
	xmax_type_message_id	message_id;
	uint64_t				message_idx;
};


} } // namespace Xmaxplatform::Chain

FC_REFLECT_DERIVED(Xmaxplatform::Chain::message_xmax, (Xmaxplatform::Basetypes::message), )
FC_REFLECT(Xmaxplatform::Chain::message_receipt, (to_code)(message_id)(message_idx))