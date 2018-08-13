/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <block.hpp>
#include <transaction_request.hpp>

namespace Xmaxplatform {
namespace Chain {


	struct block_brief
	{	
		account_name builder;
		xmax_type_block_num block_num = 0;
		xmax_type_block_id block_id;

		block_brief()
		{

		}
		block_brief(account_name bld, xmax_type_block_num num, xmax_type_block_id id)
			: block_num(num), builder(bld), block_id(id)
		{

		}
	};

	struct block_raw
	{
		xmax_type_block_num					block_num = 0;
		xmax_type_block_id					block_id;
		signed_block_header					new_header;

		builder_info						bld_info;
		uint32_t							round_slot = 0;
		builder_rule						current_builders;
		builder_rule						new_builders;
		vector<block_confirmation>			confirmations;
		bool								main_chain = false;

		xmax_type_block_num					last_block_num = 0;
		xmax_type_block_num					last_confirmed_num = 0;
		xmax_type_block_id					last_confirmed_id;
		bool								irreversible_confirmed = false;

		xmax_type_block_num					dpos_irreversible_num = 0; // make dpos irreversible block number by this block.
		xmax_type_block_id					dpos_irreversible_id;
		std::vector<block_brief>			last_block_of_builders;

		const xmax_type_block_id& prev_id() const
		{
			return new_header.previous;
		}

		void add_confirmation(const block_confirmation& conf, uint32_t skip);

		bool enough_confirmation() const;

	};

	struct block_pack : public block_raw
	{
		block_pack()
		{
		}
		bool validated = false;
		bool new_round = false;
		signed_block_ptr block;

		vector<transaction_request_ptr> transactions; // cache only, serialization is not need.


		void init_default(chain_timestamp time, const builder_info& builder, const builder_rule& cur_blders);
		void init_by_pre_pack(const block_pack& pre_pack, chain_timestamp when, bool mainchain);

		void refresh(signed_block_ptr b, const builder_rule& cur_blders, const builder_rule& new_blders, uint16_t roundslot, bool confirmed, bool mainchain, bool irr_confirmed);

		void generate_by_block(signed_block_ptr b);

		void set_next_builders(const builder_rule& next);

	private:
		void set_dpos_irreversible(xmax_type_block_num num, const xmax_type_block_id& id);
		void generate_dpos(const block_pack& pre_pack);
	};

	using block_pack_ptr = std::shared_ptr<block_pack>;
}
}
FC_REFLECT(Xmaxplatform::Chain::block_brief, (builder)(block_num)(block_id))

FC_REFLECT(Xmaxplatform::Chain::block_raw, (block_num)(block_id)(new_header)
(bld_info)(round_slot)(current_builders)(new_builders)(confirmations)(main_chain)
(last_block_num)(last_confirmed_num)(last_confirmed_id)(irreversible_confirmed)
(dpos_irreversible_num)(dpos_irreversible_id)(last_block_of_builders))

FC_REFLECT_DERIVED(Xmaxplatform::Chain::block_pack, (Xmaxplatform::Chain::block_raw), (validated)(new_round)(block))

