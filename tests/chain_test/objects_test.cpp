#define BOOST_TEST_MODULE chain_test



#include <objects/erc20_token_object.hpp>
//#include <chain_time.hpp>
//#include <publickey.hpp>

#include <asset.hpp>
#include <string_utilities.hpp>
#include <fc/log/appender.hpp>
#include "objects/account_object.hpp"
#include "basechain.hpp"

#include <boost/test/included/unit_test.hpp>
#include <objects/object_utility.hpp>



using namespace Xmaxplatform::Chain;
using namespace Xmaxplatform::Basetypes;

//*** Utility functions
namespace {

	template <typename MultiIndexType>
	static void AddErc20ObjToTable(MultiIndexType& tbl, erc20_token_object::id_type id, const std::string& token_name,		
		const std::string& owner_name, Xmaxplatform::Basetypes::share_type amount) {

		assert(token_name.size() == 3);

		erc20_token_object obj;
		//erc20_token_object obj;
		obj.id = id;
		obj.token_name = asset_symbol(MAKE_TOKEN_NAME(token_name[0], token_name[1], token_name[2]));
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		obj.token_amount = amount;
		tbl.get<by_token_and_owner>().insert(obj);
	}

	static erc20_token_multi_index_test& GetTestErc20Container() {
		static erc20_token_multi_index_test tbl;
		return tbl;
	}
	
}

namespace fc {
	extern std::unordered_map<std::string, logger>& get_logger_map();
	extern std::unordered_map<std::string, appender::ptr>& get_appender_map();
}

BOOST_AUTO_TEST_SUITE(erc20_test_suite)


BOOST_AUTO_TEST_CASE(erc20_test_add) {
	auto& tbl = GetTestErc20Container();
			
	AddErc20ObjToTable(tbl, 1, "TST", "testera", 12345);
	AddErc20ObjToTable(tbl, 2, "TST", "testerb", 54321);
	AddErc20ObjToTable(tbl, 3, "TSA", "testera", 13579);
	AddErc20ObjToTable(tbl, 4, "TSA", "testerb", 24680);
	//error case
	AddErc20ObjToTable(tbl, 5, "TST", "testera", 67890);

	BOOST_CHECK(tbl.size() == 4);
}

BOOST_AUTO_TEST_CASE(erc20_test_index) {
	auto& tbl = GetTestErc20Container();
	
	auto it = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TST", "testera"));
	BOOST_ASSERT(it != tbl.get<by_token_and_owner>().end());
	auto& obj = *it;
	BOOST_CHECK(obj.id == erc20_token_object::id_type(1));
	BOOST_CHECK(obj.token_amount == 12345);

	auto it2 = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TST", "testerb"));
	BOOST_ASSERT(it2 != tbl.get<by_token_and_owner>().end());
	auto& obj2 = *it2;
	BOOST_CHECK(obj2.id == erc20_token_object::id_type(2));
	BOOST_CHECK(obj2.token_amount == 54321);

	auto it3 = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TSA", "testera"));
	BOOST_ASSERT(it3 != tbl.get<by_token_and_owner>().end());
	auto& obj3 = *it3;
	BOOST_CHECK(obj3.id == erc20_token_object::id_type(3));
	BOOST_CHECK(obj3.token_amount == 13579);

	auto it4 = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TSA", "testerb"));
	BOOST_ASSERT(it4 != tbl.get<by_token_and_owner>().end());
	auto& obj4 = *it4;
	BOOST_CHECK(obj4.id == erc20_token_object::id_type(4));
	BOOST_CHECK(obj4.token_amount == 24680);
}



BOOST_AUTO_TEST_SUITE_END()

