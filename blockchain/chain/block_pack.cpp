/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <blockchain_exceptions.hpp>
#include <block_pack.hpp>

namespace Xmaxplatform {
namespace Chain {

	void block_raw::add_confirmation(const block_confirmation& conf, uint32_t skip)
	{
		if (NO_BIT_FLAG(skip, Config::skip_confirmation))
		{
			for (const auto& item : confirmations)
			{
				FC_ASSERT(item.verifier != conf.verifier, "confirmation had exist.");
			}

			auto key = verifiers.get_sign_key(conf.verifier);

			XMAX_ASSERT(conf.is_signer_valid(key), confirmation_validate_exception, "confirmation fail.");
		}

		confirmations.emplace_back(conf);
	}

	bool block_raw::enough_confirmation() const
	{
		int minconf = verifiers.number() * 2 / 3;
		return confirmations.size() >= minconf;
	}


	void block_pack::setup(const block_pack& pre_pack, chain_timestamp when, account_name builder, const builder_rule& rule)
	{
		block_num = pre_pack.block_num + 1;
		last_block_num = pre_pack.last_block_num;
		last_confired_num = pre_pack.last_confired_num;
		last_confired_id = pre_pack.last_confired_id;

		// build block.
		new_header.previous = pre_pack.block_id;
		new_header.timestamp = when;
		new_header.builder = builder;
	}

	void block_pack::setup()
	{
		static_cast<block_header&>(*block) = new_header;

		block_num = new_header.block_num();
		block_id = new_header.id();
	}

}
}