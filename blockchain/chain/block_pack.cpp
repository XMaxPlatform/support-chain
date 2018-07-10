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

	void block_pack::init_default(chain_timestamp time, account_name builder)
	{
		block = std::make_shared<signed_block>();
		block->timestamp = time;
		block->builder = builder;

		//static_cast<block_header&>(*block) = new_header;

		new_header.previous = empty_chain_id;
		new_header.builder = builder;

		block_num = new_header.block_num();
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


		block_brief last_brief(pre_pack.new_header.builder, pre_pack.block_num, pre_pack.block_id);

		if (pre_pack.last_block_of_builders.size() == 0)
		{
			set_dpos_irreversible(pre_pack.dpos_irreversible_num, pre_pack.dpos_irreversible_id);
			last_block_of_builders.push_back(last_brief);
		}
		else if (pre_pack.last_block_of_builders.back().builder == pre_pack.new_header.builder)// update last block info of builder.
		{
			last_block_of_builders = pre_pack.last_block_of_builders;
			set_dpos_irreversible(pre_pack.dpos_irreversible_num, pre_pack.dpos_irreversible_id);

			last_block_of_builders.back() = last_brief;
			
		}
		else //(pre_pack.last_block_of_builders.back().builder != pre_pack.new_header.builder)
		{
			if (pre_pack.last_block_of_builders.size() < Config::dpos_irreversible_num)
			{
				last_block_of_builders = pre_pack.last_block_of_builders;
				last_block_of_builders.push_back(last_brief);
				set_dpos_irreversible(pre_pack.dpos_irreversible_num, pre_pack.dpos_irreversible_id);

			}
			else //(pre_pack.last_block_of_builders.size() >= Config::dpos_irreversible_num)
			{
				// this block can make a old block become irreversible.

				// update irreversible_num 
				set_dpos_irreversible(pre_pack.last_block_of_builders.front().block_num, pre_pack.last_block_of_builders.front().block_id);


				last_block_of_builders.assign(pre_pack.last_block_of_builders.begin() + 1, pre_pack.last_block_of_builders.end());

				last_block_of_builders.push_back(last_brief);
			}
		}

	}

	void block_pack::init_by_block(signed_block_ptr b, bool confirmed)
	{
		block = b;
		new_header = static_cast<signed_block_header&>(*block);
		block_id = b->id();
		block_num = b->block_num();


		last_block_num = block_num;

		if (confirmed)
		{
			last_confired_num = block_num;
			last_confired_id = block_id;
		}
		else
		{
			last_confired_num = 0;
			last_confired_id = empty_chain_id;
		}

	}

	void block_pack::set_dpos_irreversible(xmax_type_block_num num, const xmax_type_block_id& id)
	{
		dpos_irreversible_id = id;
		dpos_irreversible_num = num;
	}
}
}