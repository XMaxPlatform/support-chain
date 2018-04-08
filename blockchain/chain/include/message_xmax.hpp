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
   message_xmax(const account_name& code, const vector<Basetypes::account_permission>& authorization, const Basetypes::func_name& type, T&& value)
      :Basetypes::message(code, type, authorization, bytes()) {
      set<T>(type, std::forward<T>(value));
   }

   message_xmax(const account_name& code, const vector<Basetypes::account_permission>& authorization, const Basetypes::func_name& type)
      :Basetypes::message(code, type, authorization, bytes()) {}

   message_xmax(const Basetypes::message& m) : Basetypes::message(m) {}

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



} } // namespace Xmaxplatform::Chain

FC_REFLECT_DERIVED(Xmaxplatform::Chain::message_xmax, (Xmaxplatform::Basetypes::message), )
