/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <cash/cash_types.hpp>
#include <cash/cash_detail.hpp>


namespace Xmaxplatform {
namespace Chain {

	class cash_transaction
	{
	public:
		uint32_t block_num;
		uint16_t index;

		cash_detail detal;
	};
}
}
FC_REFLECT(Xmaxplatform::Chain::cash_transaction, (block_num)(index) (detal))