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
   const auto& gsc = chain.get_static_config();
   memcpy(builders, gsc.current_builders.builders.data(), std::min(sizeof(account_name)*gsc.current_builders.builders.size(),(size_t)datalen));
}

time message_context_xmax::current_time() const {
   return mutable_chain.head_block_time();
}

Xmaxplatform::Chain::message_context_xmax::pending_message& message_context_xmax::get_pending_message(pending_message::handle_type handle)
{
	auto itr = boost::find_if(pending_messages, [&](const auto& msg) { return msg.handle == handle; });
	XMAX_ASSERT(itr != pending_messages.end(), tx_unknown_argument,
		"Transaction refers to non-existant/destroyed pending message");
	return *itr;
}

Xmaxplatform::Chain::message_context_xmax::pending_message& message_context_xmax::create_pending_message(const account_name& code, const func_name& type, const bytes& data)
{
	pending_message::handle_type handle = next_pending_message_serial++;
	pending_messages.emplace_back(handle, code, type, data);
	return pending_messages.back();
}

void message_context_xmax::release_pending_message(pending_message::handle_type handle)
{
	auto itr = boost::find_if(pending_messages, [&](const auto& trx) { return trx.handle == handle; });
	XMAX_ASSERT(itr != pending_messages.end(), tx_unknown_argument,
		"Transaction refers to non-existant/destroyed pending message");

	auto last = pending_messages.end() - 1;
	if (itr != last) {
		std::swap(itr, last);
	}
	pending_messages.pop_back();
}

void message_context_xmax::require_authorization(const Basetypes::account_name& account) {
    auto itr = boost::find_if(msg.authorization, [&account](const auto& auth) { return auth.account == account; });
    XMAX_ASSERT(itr != msg.authorization.end(), tx_missing_auth,
                "Transaction is missing required authorization from ${acct}", ("acct", account));
    used_authorizations[itr - msg.authorization.begin()] = true;
}

void message_context_xmax::require_authorization(const Basetypes::account_name& account,const Basetypes::authority_name& auth_name) {
    auto itr = boost::find_if(msg.authorization, [&account, &auth_name](const auto& auth) { return auth.account == account && auth.authority == auth_name; });
    XMAX_ASSERT(itr != msg.authorization.end(), tx_missing_auth,
                "Transaction is missing required authorization from ${acct} with permission for authority ${authority}", ("acct", account)("authority", auth_name));
    used_authorizations[itr - msg.authorization.begin()] = true;
}

void message_context_xmax::require_scope(const Basetypes::account_name& account)const {
   auto itr = boost::find_if(trx.scope, [&account](const auto& scope) {
      return scope == account;
   });

   XMAX_ASSERT(itr != trx.scope.end(), tx_missing_scope, "missing scope of ${account}",
	   ("account", account));

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
