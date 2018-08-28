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
		 * @brief The erc721_token_object class tracks the ERC20,ERC721 tokens for accounts
		 */
		class erc721_token_object : public Basechain::object<erc721_token_object_type, erc721_token_object> {
			OBJECT_CCTOR(erc721_token_object,(all_tokens)(token_owners)(token_urls))

			id_type id;
			Basetypes::asset_symbol token_name;
			Basetypes::account_name owner_name;		

			//---------
			shared_set<xmax_erc721_id> all_tokens;
			//---------
			shared_map<xmax_erc721_id, account_name> token_owners;
			//---------
			// Optional mapping for token URIs
			shared_map<xmax_erc721_id, string> token_urls;

			int8_t revoked = 0;
			int8_t stopmint = 0;
		};

		class erc721_token_object_test : public Basechain::object<erc721_token_object_type, erc721_token_object_test> {
			OBJECT_CCTOR(erc721_token_object_test, (minted_tokens))
		public:
			erc721_token_object_test() = default;
			
			id_type id;
			Basetypes::asset_symbol token_name;
			Basetypes::account_name owner_name;
			std::set<xmax_erc721_id> minted_tokens;
		};
		
		struct by_token_name;		

		using erc721_token_multi_index = Basechain::shared_multi_index_container<
			erc721_token_object,
			indexed_by<
			ordered_unique<tag<by_id>,
				member<erc721_token_object, erc721_token_object::id_type, &erc721_token_object::id>
			>,
			ordered_unique<tag<by_token_name>,
				member<erc721_token_object, Basetypes::asset_symbol, &erc721_token_object::token_name>
			>						
			>//indexed_by
		>;

		using erc721_token_multi_index_test = boost::multi_index_container<
			erc721_token_object_test,
			indexed_by<
			ordered_unique<tag<by_id>,
			member<erc721_token_object_test, erc721_token_object_test::id_type, &erc721_token_object_test::id>
			>,
			ordered_unique<tag<by_token_name>,
			member<erc721_token_object_test, Basetypes::asset_symbol, &erc721_token_object_test::token_name>
			>
			>//indexed_by
		>;

	}
} // namespace Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::erc721_token_object, Xmaxplatform::Chain::erc721_token_multi_index)
