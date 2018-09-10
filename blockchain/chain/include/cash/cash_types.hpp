/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>

#define MAX_CASHINPUT_SIZE (30) 
#define MAX_CASHOUTINPUT_SIZE (30) 

namespace Xmaxplatform {
namespace Chain {

	using cash_address = address;
	using cash_signature = xmax_type_signature;
	using cash_digest = xmax_type_summary;

	using cash_token = Basetypes::share_type;

	using cash_id = cash_digest;
}
}