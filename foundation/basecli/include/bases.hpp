/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <string>
#include <map>

#include <memory>
#include <functional>
#include <cassert>

namespace Basecli {

	using string = std::string;

	template<typename T>
	using dic = std::map<string, T>;

	template<typename T>
	using dicptr = std::unique_ptr<dic<T>>;

	using callback = std::function<void()>;

	class icommand;

	using commandptr = icommand*;

	class icommand
	{
	public:
		virtual ~icommand() {}

		virtual void add_option(string names, string& val, string desc, bool required) = 0;
		virtual void add_flag(string names, bool& val, string desc) = 0;
		virtual commandptr add_subcommand(string names, string desc) = 0;
		virtual void set_callback(callback c) = 0;
	};



#define CLI_ASSERT(c) assert(c)

}