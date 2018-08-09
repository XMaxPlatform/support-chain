/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <cassert>

namespace Basecli {

	using string = std::string;

	template<typename T>
	using dic = std::map<string, T>;

	template<typename T>
	using dic = std::map<string, T>;

	template<typename T>
	using dicptr = std::unique_ptr<dic<T>>;

#define CLI_ASSERT(c) assert(c)

}