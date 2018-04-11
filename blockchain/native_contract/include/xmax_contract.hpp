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

void handle_xmax_newaccount(Chain::message_context_xmax& context);
void handle_xmax_transfer(Chain::message_context_xmax& context);
} // namespace Native_contract
} // namespace Xmaxplatform

