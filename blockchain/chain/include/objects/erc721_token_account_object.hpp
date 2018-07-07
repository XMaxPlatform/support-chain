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
		 * @brief The erc721_token_account_object class tracks the ERC20,ERC721 tokens for accounts
		 */
		class erc721_token_account_object : public Basechain::object<erc721_token_account_object_type, erc721_token_account_object> {
			OBJECT_CCTOR(erc721_token_account_object,(tokens))

			id_type id;
			Basetypes::asset_symbol token_name;
			Basetypes::account_name owner_name;
			shared_set<xmax_erc721_id> tokens;
		};

		class erc721_token_account_object_test : public Basechain::object<erc721_token_account_object_type, erc721_token_account_object_test> {
		public:
			erc721_token_account_object_test() = default;

			id_type id;
			Basetypes::asset_symbol token_name;
			Basetypes::account_name owner_name;
			shared_set<xmax_erc721_id> tokens;
		};
		
		struct by_token_and_owner;		

		using erc721_token_multi_index = Basechain::shared_multi_index_container<
			erc721_token_account_object,
			indexed_by<
			ordered_unique<tag<by_id>,
				member<erc721_token_account_object, erc721_token_account_object::id_type, &erc721_token_account_object::id>
			>,
			ordered_unique<tag<by_token_and_owner>,
				composite_key<
				erc721_token_account_object,
				member<erc721_token_account_object, Basetypes::asset_symbol, &erc721_token_account_object::token_name>,
				member<erc721_token_account_object, Basetypes::account_name, &erc721_token_account_object::owner_name>
				>
			>		
			>//indexed_by
		>;

		using erc721_token_multi_index_test = boost::multi_index_container<
			erc721_token_account_object_test,
			indexed_by<
			ordered_unique<tag<by_id>,
			member<erc721_token_account_object_test, erc721_token_account_object::id_type, &erc721_token_account_object_test::id>
			>,
			ordered_unique<tag<by_token_and_owner>,
				composite_key<
				erc721_token_account_object,
				member<erc721_token_account_object_test, Basetypes::asset_symbol, &erc721_token_account_object_test::token_name>,
				member<erc721_token_account_object_test, Basetypes::account_name, &erc721_token_account_object_test::owner_name>
				>
			>
			>//indexed_by
		>;

	}
} // namespace Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::erc721_token_account_object, Xmaxplatform::Chain::erc721_token_multi_index)
