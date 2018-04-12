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

void message_context_xmax::get_active_builders(Basetypes::account_name *builders, uint32_t datalen) {
   const auto& gsc = _chain_xmax.get_static_config();
   memcpy(builders, gsc.active_builders.data(), std::min(sizeof(account_name)*gsc.active_builders.size(),(size_t)datalen));
}

time message_context_xmax::current_time() const {
   return mutable_controller.head_block_time();
}
void message_context_xmax::require_authorization(const Basetypes::account_name& account) {
    auto itr = boost::find_if(msg.authorization, [&account](const auto& auth) { return auth.account == account; });
    XMAX_ASSERT(itr != msg.authorization.end(), tx_missing_auth,
                "Transaction is missing required authorization from ${acct}", ("acct", account));
    used_authorizations[itr - msg.authorization.begin()] = true;
}

void message_context_xmax::require_authorization(const Basetypes::account_name& account,const Basetypes::permission_name& permission) {
    auto itr = boost::find_if(msg.authorization, [&account, &permission](const auto& auth) { return auth.account == account && auth.permission == permission; });
    XMAX_ASSERT(itr != msg.authorization.end(), tx_missing_auth,
                "Transaction is missing required authorization from ${acct} with permission ${permission}", ("acct", account)("permission", permission));
    used_authorizations[itr - msg.authorization.begin()] = true;
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
