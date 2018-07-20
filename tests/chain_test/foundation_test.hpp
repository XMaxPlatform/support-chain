

#include "basetypes.hpp"
#include "string_utilities.hpp"



using namespace Xmaxplatform::Basetypes;

BOOST_AUTO_TEST_SUITE(foundation_test_suite)

BOOST_AUTO_TEST_CASE(test_long_name) {
	name code = xmax::string_to_name("erc721totalsupply");
	std::string str = xmax::name_to_string(code);
	BOOST_CHECK(str.compare("erc721totalsupply") == 0);
}


BOOST_AUTO_TEST_SUITE_END()
