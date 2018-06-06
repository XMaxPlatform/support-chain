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
		typedef pair<account_name, Basetypes::name> handler_key;
		map< account_name, map<handler_key, msg_handler> >                   message_handlers;

		optional<database::session>      _pending_tx_session;
		deque<signed_transaction>         _pending_transactions;
	};

}
}