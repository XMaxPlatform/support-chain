

#include "basetypes.hpp"
#include "string_utilities.hpp"
#include <array>



using namespace Xmaxplatform::Basetypes;

BOOST_AUTO_TEST_SUITE(foundation_test_suite)

BOOST_AUTO_TEST_CASE(test_long_name) {
	
	std::array<std::string, 3> test_strs{
		"erc721totalsupply",
		"xmxoffcial@abcd.com",
		"xmax_message_test"
	};
	
	std::array<name, test_strs.size()> codes;

	for (size_t i = 0; i < test_strs.size(); i++)
	{
		name code = xmax::string_to_name(test_strs[i].c_str());
		std::string str = xmax::name_to_string(code);
		BOOST_CHECK(str.compare(test_strs[i]) == 0);
	}

}


BOOST_AUTO_TEST_SUITE_END()
