/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include<blockchain_plugin.hpp>
#include<connection_xmax.hpp>

namespace Xmaxplatform {
	using namespace Chain;
	using connection_ptr = std::shared_ptr<connection_xmax>;
	class sync_main {
	private:
		enum stages {
			liblock_catchup,
			head_catchup,
			in_sync
		};

		uint32_t       sync_known_liblock_num;
		uint32_t       sync_last_requested_num;
		uint32_t       sync_req_span;
		uint32_t       last_repeated;
		connection_ptr source;
		stages         state;

		deque<xmax_type_block_id> _blocks;
		blockchain_plugin * bc_plugin;

	public:
		sync_main(uint32_t span);
		void reset_liblock_num(std::set< connection_ptr > conns_ptr);
		void recv_handshake(connection_ptr c, const handshake_message& msg);

		static const fc::string logger_name;
		static fc::logger logger;
	};
}