/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <blockchain_types.hpp>
#include <message_context_xmax.hpp>

namespace Xmaxplatform { namespace Chain {

	using Basechain::database;

	struct pending_block
	{		
		pending_block(database::session&& s)
			:_db_session(std::move(s)) {}

		database::session                  _db_session;
		signed_block						_block;

		void push_block()
		{
			_db_session.push();
		}
	};

}
}