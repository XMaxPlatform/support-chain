/**
*  @file
*  @copyright defined in xmax/LICENSE
*/



#include <objects/authority_object.hpp>
#include <objects/transaction_object.hpp>
#include <objects/block_summary_object.hpp>
#include <objects/account_object.hpp>
#include <objects/static_config_object.hpp>
#include <objects/dynamic_states_object.hpp>

#include <objects/vote_objects.hpp>
#include <objects/resource_token_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/builder_object.hpp>
#include <objects/global_status_objects.hpp>

#include <objects/key_value_object.hpp>

#include <basechain.hpp>
#include <indexes.hpp>


namespace Xmaxplatform {
namespace Chain {

	void setup_system_indexes(Basechain::database& db) {
		db.add_index<account_index>();
		db.add_index<authority_index>();

		db.add_index<key_value_index>();
		db.add_index<keystr_value_index>();
		db.add_index<key128x128_value_index>();
		db.add_index<key64x64x64_value_index>();

		db.add_index<transaction_multi_index>();
		db.add_index<block_summary_multi_index>();

		db.add_index<static_config_multi_index>();
		db.add_index<dynamic_states_multi_index>();
		db.add_index<xmx_token_multi_index>();

		db.add_index<voter_info_index>();
		db.add_index<builder_info_index>();
		db.add_index<builder_multi_index>();
		db.add_index<resource_token_multi_index>();

		db.add_index<global_trx_status_index>();
		db.add_index<global_msg_status_index>();
	}
}
}