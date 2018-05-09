/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <sync_main.hpp>
namespace Xmaxplatform {
	fc::logger sync_main::logger;

	sync_main::sync_main(uint32_t req_span)
		:sync_known_liblock_num(0)
		, sync_last_requested_num(0)
		, sync_req_span(req_span)
		, last_repeated(0)
		, source()
		, state(in_sync)
	{
		bc_plugin = app().find_plugin<blockchain_plugin>();
	}

	void sync_main::reset_liblock_num(std::set< connection_ptr > conns_ptr) {
		sync_known_liblock_num = bc_plugin->getchain().get_dynamic_states().last_irreversible_block_num;
		sync_last_requested_num = bc_plugin->getchain().head_block_num();
		for (auto& c : conns_ptr) {
			if (c->last_handshake_recv.last_irreversible_block_num > sync_known_liblock_num) {
				sync_known_liblock_num = c->last_handshake_recv.last_irreversible_block_num;
			}
		}
	}

	

}
