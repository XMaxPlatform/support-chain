/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <bases.hpp>

namespace Basecli {
	namespace utils
	{
		void trim(string& str);

		std::vector<string> parse_names(string names);
	}
}