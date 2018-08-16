#pragma once
#include <xmaxlib/core.h>

// xmax with 4 digits of precision
#define MAIN_SYMBOL  (int64_t(8) | (uint64_t('S') << 8) | (uint64_t('U') << 16) | (uint64_t('P') << 24))
#define ASSET_SYMBOL_NONE (uint64_t)0

namespace xmax {

	struct asset 
	{
		asset(share_type a = 0, asset_symbol id = MAIN_SYMBOL)
			:amount(a), symbol(id) {}

		share_type   amount;
		asset_symbol symbol;
	};

}