/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <fc/io/raw.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/array.hpp>

namespace Xmaxplatform {
namespace Basetypes {

	using address_data = fc::array<char, 20>;

	struct address
	{
		struct binary_addr
		{
			binary_addr();
			uint32_t                 check = 0;
			address_data data;
		};

		address_data data;
		address();  
		address(const address& addr);
		address(const address_data& d);
		address(const std::string& base58);

		std::string to_string() const;

		friend bool operator == (const address& p1, const address& p2);
		friend bool operator != (const address& p1, const address& p2);
		friend bool operator < (const address& p1, const address& p2);
		friend std::ostream& operator<< (std::ostream& s, const address& k);


		static const address addr_zero;
		static const address addr_one;
	};

}
}

namespace fc {
	inline void to_variant(const Xmaxplatform::Basetypes::address& var, fc::variant& vo) { vo = var.to_string(); }
	inline void from_variant(const fc::variant& var, Xmaxplatform::Basetypes::address& vo) {
		vo = Xmaxplatform::Basetypes::address(var.get_string());
	}

} // namespace fc


FC_REFLECT(Xmaxplatform::Basetypes::address, (data))
FC_REFLECT(Xmaxplatform::Basetypes::address::binary_addr, (data)(check))