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

	inline auto MakeErcTokenIndex(Xmaxplatform::Basetypes::asset_symbol token_name, Xmaxplatform::Basetypes::account_name owner_name) {
		return std::make_tuple(token_name, owner_name);
	}

	inline auto MakeErc721TokenIndexById(Xmaxplatform::Basetypes::asset_symbol token_name, const Xmaxplatform::Chain::xmax_erc721_id& token_id) {
		return std::make_tuple(token_name, token_id);
	}

}//namespace Chain	
} //namespace Xmaxplatform