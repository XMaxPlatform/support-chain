/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>

namespace Xmaxplatform {
namespace Basetypes {

	using address_data = fc::array<char, 20>;

	struct address
	{
		address_data data;
		address();
		address(const address_data& d);

		friend bool operator == (const address& p1, const address& p2);
		friend bool operator != (const address& p1, const address& p2);
		friend bool operator < (const address& p1, const address& p2);
		friend std::ostream& operator<< (std::ostream& s, const address& k);
	};

}
}