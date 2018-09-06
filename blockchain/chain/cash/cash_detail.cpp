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
		, sig(cash.sig)
	{
		for (const auto& elem : cash.inputs)
		{
			cash_digest dg((char*)&elem.prevout.data, elem.prevout.data.size());
			cash_input in(dg, uint8_t(elem.slot));
			inputs.emplace_back(std::move(in));
		}
		for (const auto& elem : cash.outputs)
		{
			cash_output o(share_type(elem.amount), elem.to);
			outputs.emplace_back(std::move(o));
		}
	}
}
}