#pragma once
#include <shortcode.hpp>

namespace xmax {


	static _CONST_EXPR_ std::string&& name_to_string(ShortCode::name_code name)
	{
		return ShortCode::short_name::to_string(name);
	}

	static _CONST_EXPR_ ShortCode::name_code string_to_name(const char* str)
	{
		return ShortCode::short_name::to_name_code(str);
	}
}

#define XNAME(NAME) ::xmax::string_to_name(#NAME)
#define XNAMETOSTR(NAME) ::xmax::name_to_string(NAME) // Must be re-implement in because of conflict of the type 'time' with CRT in xmaxlib