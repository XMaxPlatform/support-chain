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


void xmax_system_addaccount(Chain::message_context_xmax& context);
void xmax_system_addcontract(Chain::message_context_xmax& context);
void xmax_system_adderc20(Chain::message_context_xmax& context);
void xmax_system_adderc721(Chain::message_context_xmax& context);

void xmax_system_updateauth(Chain::message_context_xmax& context);
void xmax_system_deleteauth(Chain::message_context_xmax& context);
void xmax_system_linkauth(Chain::message_context_xmax& context);
void xmax_system_unlinkauth(Chain::message_context_xmax& context);

void xmax_system_transfer(Chain::message_context_xmax& context);
void xmax_system_lock(Chain::message_context_xmax& context);
void xmax_system_unlock(Chain::message_context_xmax& context);

void xmax_system_setcode(Chain::message_context_xmax& context);

#ifdef USE_V8
void xmax_system_setjscode(Chain::message_context_xmax& context);
#endif

void xmax_system_votebuilder(Chain::message_context_xmax& context);
void xmax_system_regbuilder(Chain::message_context_xmax& context);
void xmax_system_unregbuilder(Chain::message_context_xmax& context);
void xmax_system_regproxy(Chain::message_context_xmax& context);
void xmax_system_unregproxy(Chain::message_context_xmax& context);

//ERC20 interfaces
void xmax_erc20_mint(Chain::message_context_xmax& context);
void xmax_erc20_stopmint(Chain::message_context_xmax& context);
void xmax_erc20_revoke(Chain::message_context_xmax& context);
void xmax_erc20_transferfrom(Chain::message_context_xmax& context);


//ERC721 interfaces
void xmax_erc721_mint(Chain::message_context_xmax& context);
void xmax_erc721_stopmint(Chain::message_context_xmax& context);
void xmax_erc721_transferfrom(Chain::message_context_xmax& context);
void xmax_erc721_revoke(Chain::message_context_xmax& context);





} // namespace Native_contract
} // namespace Xmaxplatform

