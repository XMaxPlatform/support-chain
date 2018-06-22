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

	void block_pack::init_default()
	{
		block = std::make_shared<signed_block>();
		static_cast<block_header&>(*block) = new_header;

		block_num = new_header.block_num();
		block_id = new_header.id();
	}

	void block_pack::init_by_pre_pack(const block_pack& pre_pack, chain_timestamp when, account_name builder, const builder_rule& rule)
	{
		block = std::make_shared<signed_block>();

		new_header.previous = pre_pack.block_id;
		new_header.timestamp = when;
		new_header.builder = builder;

		verifiers = rule;

		block_num = pre_pack.block_num + 1;
		last_block_num = pre_pack.last_block_num;
		last_confired_num = pre_pack.last_confired_num;
		last_confired_id = pre_pack.last_confired_id;


	}

	void block_pack::init_by_block(signed_block_ptr b)
	{
		block = b;
		new_header = static_cast<signed_block_header&>(*block);

		block_num = b->block_num();
		last_block_num = block_num;
		last_confired_num = block_num;
		last_confired_id = block->id();
	}

}
}