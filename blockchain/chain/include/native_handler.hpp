/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <xmaxtypes.hpp>

namespace Xmaxplatform { namespace Chain {

	class message_context_xmax;

	using native_handler = std::function<void(message_context_xmax&)>;


	enum native_scope
	{
		native_invalid = -1,
		native_system = 0,
		native_erc20 = 10,
		native_erc721 = 11,
	};
	using handler_key = pair<native_scope, func_name>;


} } // namespace Xmaxplatform::Chain
