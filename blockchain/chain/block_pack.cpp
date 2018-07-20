/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <blockchain_exceptions.hpp>
#include <chain_utils.hpp>
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

			auto key = current_builders.get_sign_key(conf.verifier);

			XMAX_ASSERT(conf.is_signer_valid(key), confirmation_validate_exception, "confirmation fail.");
		}

		confirmations.emplace_back(conf);
	}

	bool block_raw::enough_confirmation() const
	{
		int minconf = current_builders.number() * 2 / 3;
		return confirmations.size() >= minconf;
	}

	void block_pack::init_default(chain_timestamp time, const builder_info& builder, const builder_rule& cur_blders)
	{
		block = std::make_shared<signed_block>();

		block->timestamp = time;
		block->builder = builder.builder_name;
		block->previous = empty_chain_id;

		new_header.timestamp = time;
		new_header.builder = builder.builder_name;
		new_header.previous = empty_chain_id;

		block_num = new_header.block_num();

		bld_info = builder;
		round_slot = 0;
		current_builders = cur_blders;
		main_chain = true;

		validated = false;
		new_round = true;


	}

	void block_pack::init_by_pre_pack(const block_pack& pre_pack, chain_timestamp when, bool mainchain)
	{
		block = std::make_shared<signed_block>();

		// generate confirm
		last_block_num = pre_pack.last_block_num;
		last_confirmed_num = pre_pack.last_confirmed_num;
		last_confirmed_id = pre_pack.last_confirmed_id;

		// generate block info.
		uint32_t delta_slot = utils::get_delta_slot_at_time(pre_pack.new_header.timestamp, when);

		block_num = pre_pack.block_num + 1;
		uint32_t new_slot = pre_pack.round_slot + delta_slot;

		const builder_info& current_builder = utils::select_builder(pre_pack.current_builders, pre_pack.new_builders, new_slot);

		new_header.previous = pre_pack.block_id;
		new_header.timestamp = when;
		new_header.builder = current_builder.builder_name;

		// -----------------------------------
		main_chain = mainchain;
		round_slot = new_slot;
		new_round = round_slot < Config::blocks_per_round;
		if (new_round)
		{
			if (!pre_pack.new_builders.is_empty())
			{
				current_builders = pre_pack.new_builders;
				// new_builders empty.
			}
			else
			{
				current_builders = pre_pack.current_builders;
				new_builders = pre_pack.new_builders;
			}
			round_slot = round_slot % Config::blocks_per_round;
		}
		else
		{
			current_builders = pre_pack.current_builders;
			new_builders = pre_pack.new_builders;
		}

		// generate dpos
		generate_dpos(pre_pack);
	}

	void block_pack::refresh(signed_block_ptr b, const builder_rule& cur_blders, const builder_rule& new_blders, uint16_t roundslot, bool confirmed, bool mainchain)
	{		
		// generate block info.
		block = b;

		block_id = b->id();
		block_num = b->block_num();

		last_block_num = block_num;

		// generate confirm
		if (confirmed)
		{
			last_confirmed_num = block_num;
			last_confirmed_id = block_id;
		}
		else
		{
			last_confirmed_num = 0;
			last_confirmed_id = empty_chain_id;
		}

		current_builders = cur_blders;
		new_builders = new_blders;
		round_slot = roundslot;

		main_chain = mainchain;

		generate_by_block(b);
	}

	void block_pack::generate_by_block(signed_block_ptr b)
	{
		FC_ASSERT(b->builder == new_header.builder, "wrong builder number.");
		FC_ASSERT(b->previous == new_header.previous, "wrong previous id.");
		FC_ASSERT(b->timestamp == new_header.timestamp, "wrong time stamp.");

		if (b->next_builders)
		{
			new_builders = *b->next_builders;
		}

		new_header = static_cast<signed_block_header&>(*b);

		block = b;
	}

	void block_pack::set_next_builders(const builder_rule& next)
	{
		FC_ASSERT(current_builders.version + 1 == next.version, "wrong builder version.");
		FC_ASSERT(new_builders.number() == 0, "wrong builder number.");

		new_header.next_builders = next;
		new_builders = next;
	}

	void block_pack::set_dpos_irreversible(xmax_type_block_num num, const xmax_type_block_id& id)
	{
		dpos_irreversible_id = id;
		dpos_irreversible_num = num;
	}

	void block_pack::generate_dpos(const block_pack& pre_pack)
	{
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
			if (pre_pack.last_block_of_builders.size() < Config::dpos_irreversible_need)
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
}
}