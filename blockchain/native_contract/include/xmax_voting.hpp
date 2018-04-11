/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <xmaxtypes.hpp>

namespace Xmaxplatform {namespace Chain{
        class message_context_xmax;
}}

namespace Xmaxplatform { ///< xmax native xmx token contract
namespace Native_contract {

    namespace Chain = ::Xmaxplatform::Chain;
    namespace Types = ::Xmaxplatform::Basetypes;

    class voting
    {
    public:

        static void increase_voting_power( Chain::message_context_xmax& context, Types::account_name acnt, Types::share_type amount );
        static void decrease_voting_power( Chain::message_context_xmax& context, Types::account_name acnt, Types::share_type amount );

        static void vote_producer(Chain::message_context_xmax& context);
        static void reg_producer(Chain::message_context_xmax&);
        static void unreg_prod(Chain::message_context_xmax&);
        static void reg_proxy(Chain::message_context_xmax&);
        static void unreg_proxy(Chain::message_context_xmax&);

    };

} // namespace Native_contract
} // namespace Xmaxplatform