/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <include/xmaxtypes.hpp>

namespace Xmaxplatform { namespace Config {
using Basetypes::uint16;
using Basetypes::uint32;
using Basetypes::uint64;
using Basetypes::uint128;
using Basetypes::share_type;
using Basetypes::asset;
using Basetypes::account_name;
using Basetypes::permission_name;

        const static char key_prefix[] = "XMX";
        const static int blocks_per_round = 1;
        const static account_name xmax_contract_name = STN(xmax);
} } // namespace Xmaxplatform::Config
