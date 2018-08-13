/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#pragma once
#include <include/xmaxtypes.hpp>

namespace chain_get
{
	std::string block_struct_info();

	std::string list_block_size(const std::string& block_log_dir, int64_t begin_num, int64_t block_count);
}