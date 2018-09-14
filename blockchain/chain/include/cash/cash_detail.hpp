/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <cash/cash_types.hpp>
#include <vector>

namespace Xmaxplatform {
namespace Chain {

#define paytype_enum  uint16_t
	enum paytype : paytype_enum
	{
		pay_none = 0,
		pay_to_addr = 1,
		mint_to_addr = 2,
	};

	using fpaytypevalue = paytype_enum;

	const uint8_t mint_slot_flag = 0xff;

	struct cash_input {
		cash_input() = default;
		cash_input(uint32_t _num, uint16_t _idx, uint8_t _slot)
			: block_num(_num), trx_idx(_idx), slot(_slot)
		{

		}
		uint32_t block_num;
		uint16_t trx_idx;
		uint8_t slot;
	};

	using cash_inputs = std::vector<cash_input>;

	struct cash_output {
		cash_output()
			: to(address::addr_zero)
		{

		}
		cash_output(const address& _to, const uint64_t& _amount)
			: amount(_amount), to(_to) {}
		address		to;
		cash_token	amount = 0;
	};
	using cash_outputs = std::vector<cash_output>;


	struct cash_attachment {
		cash_attachment() = default;
		cash_attachment(const chain_timestamp& locktime)
			: locktime(locktime) {}

		chain_timestamp locktime;
	};

	struct cash_detail 
	{
		paytype pay;
		cash_inputs inputs;
		cash_outputs outputs;
		cash_attachment attachment;

		cash_detail()
			: pay(paytype::pay_none)
		{

		}

		cash_detail(paytype pt, const  Basetypes::pay_cash& cash);
		cash_digest             digest() const;
	};

	struct signed_cash_detail
	{
		cash_detail detail;
		cash_signature sig;
	};
}
}

FC_REFLECT(Xmaxplatform::Chain::cash_input, (block_num)(trx_idx)(slot))
FC_REFLECT(Xmaxplatform::Chain::cash_output, (to)(amount))

namespace fc {
namespace raw {
	template<typename Stream>
	inline void pack(Stream& s, const Xmaxplatform::Chain::cash_inputs& value)
	{
		uint8_t size = value.size();
		FC_ASSERT(size <= MAX_CASHINPUT_SIZE);

		fc::raw::pack(s, size);

		auto itr = value.begin();
		auto end = value.end();
		while (itr != end) {
			fc::raw::pack(s, *itr);
			++itr;
		}
	}
	template<typename Stream>
	inline void unpack(Stream& s, Xmaxplatform::Chain::cash_inputs& value)
	{
		uint8_t size;
		fc::raw::unpack(s, size);
		FC_ASSERT(size < MAX_CASHINPUT_SIZE);
		value.resize(size);
		auto itr = value.begin();
		auto end = value.end();
		while (itr != end) {
			fc::raw::unpack(s, *itr);
			++itr;
		}
	}

	template<typename Stream>
	inline void pack(Stream& s, const Xmaxplatform::Chain::cash_outputs& value)
	{
		uint8_t size = value.size();
		FC_ASSERT(size <= MAX_CASHOUTINPUT_SIZE);
		fc::raw::pack(s, size);

		auto itr = value.begin();
		auto end = value.end();
		while (itr != end) {
			fc::raw::pack(s, *itr);
			++itr;
		}
	}
	template<typename Stream>
	inline void unpack(Stream& s, Xmaxplatform::Chain::cash_outputs& value)
	{
		uint8_t size;
		fc::raw::unpack(s, size);
		FC_ASSERT(size <= MAX_CASHOUTINPUT_SIZE);
		value.resize(size);
		auto itr = value.begin();
		auto end = value.end();
		while (itr != end) {
			fc::raw::unpack(s, *itr);
			++itr;
		}
	}
}
}

namespace fc {
namespace raw {
	template<typename Stream>
	inline void pack(Stream& s, const Xmaxplatform::Chain::cash_detail& cash)
	{
		fc::raw::xpackanyvalue(s, cash.pay);			
		fc::raw::pack(cash.attachment);

		switch (cash.pay)
		{
		case Xmaxplatform::Chain::paytype::mint_to_addr:
		{
			fc::raw::pack(cash.inputs[0]);
			fc::raw::pack(cash.outputs[0]);

		}
		case Xmaxplatform::Chain::paytype::pay_to_addr:
		{
			fc::raw::pack(cash.inputs);
			fc::raw::pack(cash.outputs);
		}
		default:
			break;
		}

	}
	template<typename Stream>
	inline void unpack(Stream& s, Xmaxplatform::Chain::cash_detail& cash)
	{
		fc::raw::xunpackanyvalue(s, cash.pay);
		fc::raw::unpack(cash.attachment);

		switch (cash.pay)
		{
		case Xmaxplatform::Chain::paytype::mint_to_addr:
		{
			cash.inputs.resize(1);
			cash.outputs.resize(1);
			fc::raw::unpack(cash.inputs[0]);
			fc::raw::unpack(cash.outputs[0]);

		}
		case Xmaxplatform::Chain::paytype::pay_to_addr:
		{
			fc::raw::unpack(cash.inputs);
			fc::raw::unpack(cash.outputs);
		}
		default:
			break;
		}
	}
}}