/**
*  @file
*  @copyright defined in xmax/LICENSE.txt
*/
#pragma once
#include <string>
#include <asset.hpp>
#include <string_utilities.hpp>


namespace Xmaxplatform { namespace Chain {

	inline auto MakeErcTokenIndex(const std::string& token_name, const std::string& owner_name) {
		return std::make_tuple(Xmaxplatform::Basetypes::asset_symbol(MAKE_TOKEN_NAME(token_name[0], token_name[1], token_name[2])),
			xmax::string_to_name(owner_name.c_str()));
	}

}//namespace Chain	
} //namespace Xmaxplatform