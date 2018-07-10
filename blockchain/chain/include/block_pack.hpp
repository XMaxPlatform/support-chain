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
		builder_rule						verifiers;
		vector<block_confirmation>			confirmations;

		bool								main_chain = false;
		xmax_type_block_num					last_block_num = 0;
		xmax_type_block_num					last_confired_num = 0;
		xmax_type_block_id					last_confired_id;

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

		signed_block_ptr block;
		vector<transaction_request_ptr> transactions;


		void init_default(chain_timestamp time, account_name builder);
		void init_by_pre_pack(const block_pack& pre_pack, chain_timestamp when, account_name builder, const builder_rule& rule);
		void init_by_block(signed_block_ptr b, bool confirmed);

	private:
		void set_dpos_irreversible(xmax_type_block_num num, const xmax_type_block_id& id);
	};

	using block_pack_ptr = std::shared_ptr<block_pack>;
}
}
FC_REFLECT(Xmaxplatform::Chain::block_brief, (builder)(block_num)(block_id))

FC_REFLECT(Xmaxplatform::Chain::block_raw, (block_num)(block_id)(new_header)(verifiers)(confirmations)
(main_chain)(last_block_num)(last_confired_num)(last_confired_id)
(dpos_irreversible_num)(dpos_irreversible_id)(last_block_of_builders))

FC_REFLECT_DERIVED(Xmaxplatform::Chain::block_pack, (Xmaxplatform::Chain::block_raw), (block))

