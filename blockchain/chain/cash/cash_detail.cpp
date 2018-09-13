/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <cash/cash_detail.hpp>


namespace Xmaxplatform {
namespace Chain {

	cash_detail::cash_detail(paytype pt, const Basetypes::pay_cash& cash)
		: pay(pt)
		, attachment(chain_timestamp::create(chain_timestamp::stamp_type(cash.attachment.locktime)))
	{
		for (const auto& elem : cash.inputs)
		{
			cash_digest dg((char*)&elem.prevout.data, elem.prevout.data.size());
			cash_input in(dg, uint8_t(elem.slot));
			inputs.emplace_back(std::move(in));
		}
		for (const auto& elem : cash.outputs)
		{
			cash_output o(elem.to, Basetypes::share_type(elem.amount));
			outputs.emplace_back(std::move(o));
		}
	}

	cash_digest cash_detail::digest() const
	{
		return cash_digest::hash(*this);
	}
}
}