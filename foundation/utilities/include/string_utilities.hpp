#pragma once
#include <shortcode.hpp>

namespace xmax {
		
	static _CONST_EXPR_ std::string&& name_to_string(Xmaxplatform::Basetypes::name::name_code_type name)
	{
		return Xmaxplatform::Basetypes::name::to_string(name);
	}

	static _CONST_EXPR_ Xmaxplatform::Basetypes::name::name_code_type string_to_name(const char* str)
	{
		return Xmaxplatform::Basetypes::name::to_name_code(str);
	}
}

#define XNAME(NAME) ::xmax::string_to_name(#NAME)
#define XNAMETOSTR(NAME) ::xmax::name_to_string(NAME) // Must be re-implement in because of conflict of the type 'time' with CRT in xmaxlib