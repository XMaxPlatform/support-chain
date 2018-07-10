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
		{
		}

		database::session					db_session;
		block_pack_ptr						pack;

		std::vector<message_receipt>		message_receipts;

		void push_db()
		{
			db_session.push();
		}
	};

}
}