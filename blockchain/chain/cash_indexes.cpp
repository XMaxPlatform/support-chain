/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <basechain.hpp>
#include <indexes.hpp>
#include <objects/xmx_cash_object.hpp>
#include <objects/linked_cash_object.hpp>


namespace Xmaxplatform {
namespace Chain {

	void setup_cash_indexes(Basechain::database& db) {
		db.add_index<xmx_cash_index>();
		db.add_index<linked_cash_index>();
	}
}
}