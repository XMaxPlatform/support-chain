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



using namespace Xmaxplatform::Chain;
using namespace Xmaxplatform::Basetypes;

//*** Utility functions


BOOST_AUTO_TEST_SUITE(erc20_test_suite)


BOOST_AUTO_TEST_CASE(erc20_test_add) {
	erc20_token_multi_index_test tbl;
	erc20_token_object obj;
	//erc20_token_object obj;
	obj.id = 1;
	obj.token_name = asset_symbol(xmax::string_to_name("TST"));
	obj.owner_name = ::string_to_name("testera");
	obj.xmx_token = 12345;	
	tbl.get<by_token_and_owner>().insert(obj);
	BOOST_CHECK(tbl.size() == 1);
}


BOOST_AUTO_TEST_SUITE_END()

namespace fc {
	extern std::unordered_map<std::string, logger>& get_logger_map();
	extern std::unordered_map<std::string, appender::ptr>& get_appender_map();
}

namespace {
	class AutoReleaser {
	public:
		~AutoReleaser() {
			fc::get_logger_map().clear();
			fc::get_appender_map().clear();
		}
	} autoReleaser;
}