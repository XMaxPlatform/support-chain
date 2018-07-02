/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
namespace Xmaxplatform {
namespace Chain {
	

	class message_response
	{
	public:
		xmax_type_transaction_id owner_id;
	};


	class transaction_response
	{
	public:
		std::vector<message_response> message_responses;
	};


	using transaction_response_ptr = std::shared_ptr<transaction_response>;

}
}
FC_REFLECT(Xmaxplatform::Chain::message_response,(owner_id))
FC_REFLECT(Xmaxplatform::Chain::transaction_response,(message_responses))