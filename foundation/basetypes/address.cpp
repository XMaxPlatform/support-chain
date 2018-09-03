/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <fc/crypto/ripemd160.hpp>
#include <fc/crypto/base58.hpp>
#include <address.hpp>

#define ADDR_PREFIX "0x"

namespace Xmaxplatform {
namespace Basetypes {


	address::binary_addr::binary_addr()
	{

	}

	address::address()
		: data()
	{

	}

	address::address(const address& addr)
		: data(addr.data)
	{

	}

	address::address(const address_data& d)
		: data(d)
	{

	}

	address::address(const std::string& base58)
	{
		// TODO:  Refactor syntactic checks into static is_valid()
		//        to make public_key API more similar to address API
		std::string prefix(ADDR_PREFIX);

		const size_t prefix_len = prefix.size();
		FC_ASSERT(base58.size() > prefix_len);
		FC_ASSERT(base58.substr(0, prefix_len) == prefix, "", ("base58str", base58));
		auto bin = fc::from_base58(base58.substr(prefix_len));
		auto bin_key = fc::raw::unpack<binary_addr>(bin);
		data = bin_key.data;
		FC_ASSERT(fc::ripemd160::hash(data.data, (uint32_t)data.size())._hash[0] == bin_key.check);
	}

	std::string address::to_string() const
	{
		binary_addr k;
		k.data = data;
		k.check = fc::ripemd160::hash(k.data.data, (uint32_t)k.data.size())._hash[0];
		auto data = fc::raw::pack(k);
		return ADDR_PREFIX + fc::to_base58(data.data(), data.size());
	}

	bool operator == (const address& p1, const address& p2)
	{
		return p1.data == p2.data;
	}

	bool operator != (const address& p1, const address& p2)
	{
		return p1.data != p2.data;
	}

	bool operator <(const address& p1, const address& p2)
	{
		return p1.data < p2.data;
	};

	std::ostream& operator<<(std::ostream& s, const address& k) {
		s << "address(" << k.to_string() << ')';
		return s;
	}
}
}