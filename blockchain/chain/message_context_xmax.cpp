/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <blockchain_exceptions.hpp>
#include <objects/key_value_object.hpp>
#include <chain_xmax.hpp>

#include <parallel_markers.hpp>

#include <fc/bitutil.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/range/algorithm/find_if.hpp>

namespace Xmaxplatform { namespace Chain {

void message_context_xmax::get_active_producers(Basetypes::account_name* producers, uint32_t datalen) {
   const auto& gsc = _chain_xmax.get_static_config();
   memcpy(producers, gsc.active_builders.data(), std::min(sizeof(account_name)*gsc.active_builders.size(),(size_t)datalen));
}

void message_context_xmax::require_scope(const Basetypes::account_name& account)const {
   auto itr = boost::find_if(trx.scope, [&account](const auto& scope) {
      return scope == account;
   });

}

void message_context_xmax::require_recipient(const Basetypes::account_name& account) {
   if (account == msg.code)
      return;

   auto itr = boost::find_if(notified, [&account](const auto& recipient) {
      return recipient == account;
   });

   if (itr == notified.end()) {
      notified.push_back(account);
   }
}









} } // namespace Xmaxplatform::chain
