/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <cash/cash_utils.hpp>
#include <chain_utils.hpp>
#include <objects/mint_cash_object.hpp>
#include <message_context_xmax.hpp>
#include <message_xmax.hpp>
#include <xmax_contract.hpp>

namespace Xmaxplatform {
namespace Native_contract {
	using namespace Chain;
	namespace Config = ::Xmaxplatform::Config;
	namespace Chain = ::Xmaxplatform::Chain;
	using namespace ::Xmaxplatform::Basetypes;

	static const mint_cash_object& create_mint(const cash_mint& mint, Basechain::database&  db)
	{
		return db.create<mint_cash_object>([&](mint_cash_object& obj) {
			obj.mint = mint;
		});
	}

	static const mint_cash_object& get_mint_object(const cash_mint& mint, Basechain::database& db)
	{
		const mint_cash_object* obj = db.find<mint_cash_object, by_sequence>(mint.sequence);

		// for test only, would be deleted.
		{
			if (nullptr == obj)
			{
				return create_mint(mint, db);
			}
		}
		XMAX_ASSERT(obj->mint == mint, message_validate_exception, "bad mint info.");

		return *obj;
	}


	void xmax_cash_mint(Chain::message_context_xmax& context)
	{
		Types::mintcash msg = context.msg.as<Types::mintcash>();

		cash_mint mint(msg.mintdetail.sequence, msg.mintdetail.owner, cash_token(msg.mintdetail.amount));

		cash_digest mintdigest = mint.digest();

		// check mint sign.

		cash_address addr = utils::to_address(msg.mintdetail.sig, mintdigest);

		XMAX_ASSERT(addr == mint.owner, message_validate_exception, "bad sig for cash owner.");
	
		// check mint exist.
		const mint_cash_object& mintobj = get_mint_object(mint, context.mutable_db);


		// cash must be 1 input, and 1 output.
		// input's prevout is mint digest.

		XMAX_ASSERT(msg.paydetail.inputs.size() == 1, message_precondition_exception, "cash.inputs.size() must be 1.");
		XMAX_ASSERT(msg.paydetail.outputs.size() == 1, message_precondition_exception, "cash.outputs.size() must be 1.");
		XMAX_ASSERT(msg.paydetail.outputs[0].to != address::addr_zero, message_precondition_exception, "error output address.");

		XMAX_ASSERT(msg.paydetail.outputs[0].amount == mintobj.mint.token, message_validate_exception, "input amount must be equal to output amount.");

		cash_digest prevout(msg.paydetail.inputs[0].prevout);

		XMAX_ASSERT(prevout == mintdigest, message_validate_exception, "input's prevout must be equal to mint digest.");

		cash_detail detail(paytype::mint_to_addr, msg.paydetail);

		cash_digest digest = detail.digest();

		// check mint sign.

		cash_address addr = utils::to_address(msg.paydetail.sig, digest);

		XMAX_ASSERT(addr == mintobj.mint.owner, message_validate_exception, "bad sig for cash owner.");



	}

}
}