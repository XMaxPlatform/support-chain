/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <block_pack.hpp>
#include <message_context_xmax.hpp>

namespace Xmaxplatform { namespace Chain {

	using Basechain::database;

	struct pending_block
	{		
		pending_block(database::session&& s)
			:db_session(std::move(s)) 
			, pack(std::make_shared<block_pack>())
		{
		}

		database::session					db_session;
		block_pack_ptr						pack;

		void push_block()
		{
			db_session.push();
		}
	};

}
}