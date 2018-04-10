/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <xmax_contract.hpp>

#include <chain_xmax.hpp>
#include <message_context_xmax.hpp>
#include <message_xmax.hpp>
#include <blockchain_exceptions.hpp>

#include <objects/account_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/static_config_object.hpp>

//#include <wasm_interface.hpp>
#include <abi_serializer.hpp>


namespace Xmaxplatform {
    namespace Native_contract {
using namespace Chain;
namespace config = ::Xmaxplatform::Config;
namespace Chain = ::Xmaxplatform::Chain;
using namespace ::Xmaxplatform::Basetypes;


void handle_xmax_newaccount(message_context_xmax& context) {
   auto create = context.msg.as<Types::newaccount>();

   auto& db = context.mutable_db;

   auto existing_account = db.find<account_object, by_name>(create.name);
   XMAX_ASSERT(existing_account == nullptr, account_name_exists_exception,
              "Cannot create account named ${name}, as that name is already taken",
              ("name", create.name));

   const auto& new_account = db.create<account_object>([&create, &db](account_object& a) {
      a.name = create.name;
      a.creation_date = db.get(dynamic_states_object::id_type()).time;
   });

   const auto& creatorToken = context.mutable_db.get<xmx_token_object, by_owner_name>(create.creator);

   XMAX_ASSERT(creatorToken.xmx_token >= create.deposit.amount, message_validate_exception,
              "Creator '${c}' has insufficient funds to make account creation deposit of ${a}",
              ("c", create.creator)("a", create.deposit));

   context.mutable_db.modify(creatorToken, [&create](xmx_token_object& b) {
      b.xmx_token -= create.deposit.amount;
   });

   context.mutable_db.create<xmx_token_object>([&create](xmx_token_object& b) {
      b.owner_name = create.name;
      b.xmx_token = 0; //create.deposit.amount; TODO: make sure we credit this in @staked
   });

}

void handle_xmax_transfer(message_context_xmax& context) {
   auto transfer = context.msg.as<Types::transfer>();

   try {
      XMAX_ASSERT(transfer.amount > 0, message_validate_exception, "Must transfer a positive amount");
      context.require_scope(transfer.to);
      context.require_scope(transfer.from);

      context.require_recipient(transfer.to);
      context.require_recipient(transfer.from);
   } FC_CAPTURE_AND_RETHROW((transfer))


   try {
      auto& db = context.mutable_db;
      const auto& from = db.get<xmx_token_object, by_owner_name>(transfer.from);

      XMAX_ASSERT(from.xmx_token >= transfer.amount, message_precondition_exception, "Insufficient Funds",
                 ("from.xmx_token",from.xmx_token)("transfer.amount",transfer.amount));

      const auto& to = db.get<xmx_token_object, by_owner_name>(transfer.to);
      db.modify(from, [&](xmx_token_object& a) {
         a.xmx_token -= share_type(transfer.amount);
      });
      db.modify(to, [&](xmx_token_object& a) {
         a.xmx_token += share_type(transfer.amount);
      });
   } FC_CAPTURE_AND_RETHROW( (transfer) ) 
}
    } // namespace Native
} // namespace Xmaxplatform

