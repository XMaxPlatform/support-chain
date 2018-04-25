/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <xmaxtypes.hpp>
#include <builder_rule.hpp>

namespace Xmaxplatform {namespace Chain{
        class message_context_xmax;
}}

namespace Basechain{
	class database;
}


namespace Xmaxplatform { ///< xmax native xmx token contract
namespace Native_contract {

    namespace Chain = ::Xmaxplatform::Chain;
    namespace Types = ::Xmaxplatform::Basetypes;

    class xmax_voting
    {
    public:

        static void increase_votes(Chain::message_context_xmax &context, Types::account_name acnt,
                                   Types::share_type amount);
        static void decrease_votes(Chain::message_context_xmax &context, Types::account_name acnt,
                                   Types::share_type amount);

        static void vote_builder(Chain::message_context_xmax& context);
        static void reg_builder(Chain::message_context_xmax& context);
        static void unreg_builder(Chain::message_context_xmax& context);
        static void reg_proxy(Chain::message_context_xmax& context);
        static void unreg_proxy(Chain::message_context_xmax& context);

		static Chain::xmax_builders next_round(Basechain::database& db);

    };

} // namespace Native_contract
} // namespace Xmaxplatform