/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <address.hpp>

namespace Xmaxplatform {
namespace Basetypes {

	address::address()
		: data()
	{

	}

	address::address(const address_data& d)
		£ºdata(d)
	{

	}

	bool operator == (const address& p1, const address& p2)
	{
		return p1.key_data == p2.key_data;
	}

	bool operator != (const address& p1, const address& p2)
	{
		return p1.key_data != p2.key_data;
	}

	bool operator <(const address& p1, const address& p2)
	{
		return p1.key_data < p2.key_data;
	};

	std::ostream& operator<<(std::ostream& s, const address& k) {
		s << "address(" << std::string(k) << ')';
		return s;
	}
}
}