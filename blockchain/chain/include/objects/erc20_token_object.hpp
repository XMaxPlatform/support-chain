/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <blockchain_types.hpp>
#include "multi_index_includes.hpp"
#include <basechain.hpp>

namespace Xmaxplatform {
	namespace Chain {


		/**
		 * @brief The erc20_token_object class tracks the ERC20,ERC721 tokens for accounts
		 */
		class erc20_token_object : public Basechain::object<erc20_token_object_type, erc20_token_object> {
			OBJECT_CCTOR(erc20_token_object)

			id_type id;
			Basetypes::asset_symbol token_name;
			Basetypes::account_name owner_name;
			Basetypes::share_type xmx_token = 0;
		};
		
		struct by_token_and_owner;

		using erc20_token_multi_index = Basechain::shared_multi_index_container<
			erc20_token_object,
			indexed_by<
			ordered_unique<tag<by_id>,
				member<erc20_token_object, erc20_token_object::id_type, &erc20_token_object::id>
			>,
			ordered_unique<tag<by_token_and_owner>,
				composite_key<
				erc20_token_object,
				member<erc20_token_object, Basetypes::asset_symbol, &erc20_token_object::token_name>,
				member<erc20_token_object, Basetypes::account_name, &erc20_token_object::owner_name>
				>
			>
			>
		>;

		using erc20_token_multi_index_test = boost::multi_index_container<
			erc20_token_object,
			indexed_by<
			ordered_unique<tag<by_id>,
			member<erc20_token_object, erc20_token_object::id_type, &erc20_token_object::id>
			>,
			ordered_unique<tag<by_token_and_owner>,
			composite_key<
			erc20_token_object,
			member<erc20_token_object, Basetypes::asset_symbol, &erc20_token_object::token_name>,
			member<erc20_token_object, Basetypes::account_name, &erc20_token_object::owner_name>
			>
			>
			>
		>;

	}
} // namespace Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::erc20_token_object, Xmaxplatform::Chain::erc20_token_multi_index)
