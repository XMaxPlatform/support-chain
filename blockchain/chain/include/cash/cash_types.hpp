/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>


namespace Xmaxplatform {
namespace Chain {

	using cash_address = address;
	using cash_signature = xmax_type_signature;
	using cash_digest = xmax_type_summary;

	using cash_token = uint64_t;

	using cash_id = cash_digest;
}
}