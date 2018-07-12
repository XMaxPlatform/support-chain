/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <blockchain_types.hpp>
#include <transaction.hpp>
#include <message_xmax.hpp>
namespace Xmaxplatform {
namespace Chain {

	class message_response
	{
	public:
		xmax_type_transaction_id	owner_id;
		message_xmax				msg_body;
		message_receipt				msg_receipt;
		string						out_string;

		std::vector<message_response> sub_message;
	};


	class transaction_response
	{
	public:
		fc::optional<transaction_receipt_header> receipt;

		std::vector<message_response> message_responses;

		fc::optional<fc::exception>                error;
		std::exception_ptr                         error_ptr;
	};


	using transaction_response_ptr = std::shared_ptr<transaction_response>;

}
}
FC_REFLECT(Xmaxplatform::Chain::message_response, (owner_id)(msg_body)(msg_receipt)(out_string)(sub_message))
FC_REFLECT(Xmaxplatform::Chain::transaction_response,(receipt)(message_responses))