/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <cash/cash_utils.hpp>
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




	}

}
}