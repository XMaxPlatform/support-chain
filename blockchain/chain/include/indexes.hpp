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

	void setup_system_indexes(Basechain::database& db);
	void setup_erc_indexes(Basechain::database& db);
	void setup_coin_indexes(Basechain::database& db);
}
}