/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <objects/account_object.hpp>


namespace Xmaxplatform {
namespace Chain {

	void account_object::set_contract(const char* code, size_t len, const Xmaxplatform::Basetypes::abi& abi) {

		if (contract.empty())
		{
			contract.init();
		}

		contract->code_version = fc::sha256::hash(code, len);

		// Added resize(0) here to avoid bug in boost vector container
		contract->code.resize(0);
		contract->code.resize(len);
		memcpy(contract->code.data(), code, len);


		// Added resize(0) here to avoid bug in boost vector container
		contract->abi.resize(0);
		contract->abi.resize(fc::raw::pack_size(abi));
		fc::datastream<char*> ds(contract->abi.data(), contract->abi.size());
		fc::raw::pack(ds, abi);
	}
}
}