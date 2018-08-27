/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <objects/erc20_token_object.hpp>
#include <objects/erc20_token_account_object.hpp>
#include <objects/erc721_token_object.hpp>
#include <objects/erc721_token_account_object.hpp>

#include <objects/key_value_object.hpp>

#include <basechain.hpp>
#include <indexes.hpp>


namespace Xmaxplatform {
namespace Chain {

	void setup_erc_indexes(Basechain::database& db) {

		db.add_index<erc20_token_multi_index>();
		db.add_index<erc20_token_account_multi_index>();
		db.add_index<erc721_token_multi_index>();
		db.add_index<erc721_token_account_multi_index>();

	}
}
}