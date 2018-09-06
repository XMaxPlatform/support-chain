/**
*  @file
*  @copyright defined in xmax/LICENSE
*/


#include <basechain.hpp>
#include <indexes.hpp>
#include <xmax_indexes.hpp>



namespace Xmaxplatform {
namespace Chain {

	void setup_xmax_indexes(Basechain::database& db) {

		setup_system_indexes(db);
		setup_erc_indexes(db);
		setup_cash_indexes(db);
	}
}
}