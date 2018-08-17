/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <blockchain_types.hpp>

namespace Basechain {
	class database;
}

namespace Xmaxplatform {

namespace Chain {

	void setup_xmax_indexes(Basechain::database& db);
}
}