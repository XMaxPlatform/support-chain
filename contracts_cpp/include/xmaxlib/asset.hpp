/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <xmaxlib/core.h>

// xmax with 4 digits of precision
#define XMX_SYMBOL  (int64_t(4) | (uint64_t('X') << 8) | (uint64_t('M') << 16) | (uint64_t('X') << 24))
#define ASSET_SYMBOL_NONE (uint64_t)0

namespace xmax {

	struct asset 
	{
		asset(share_type a = 0, asset_symbol id = XMX_SYMBOL)
			:amount(a), symbol(id) {}

		share_type   amount;
		asset_symbol symbol;
	};

}