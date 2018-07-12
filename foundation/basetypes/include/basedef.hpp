/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <vector>
#include <array>
#include <string>
#include <functional>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fc/variant.hpp>
#include <fc/crypto/base64.hpp>
#include <fc/crypto/hex.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/io/datastream.hpp>
#include <fc/time.hpp>
#include <fc/fixed_string.hpp>
#include <fc/string.hpp>

#include <fc/reflect/reflect.hpp>

#if WIN32
#include <fc/int128.hpp>
#endif

#if WIN32
#	if _MSC_VER < 1914
#		define CONST_EXPR 
#	else
#		define CONST_EXPR constexpr
#	endif
#else
#		define CONST_EXPR constexpr
#endif

namespace Xmaxplatform {
namespace Basetypes {


	using namespace boost::multiprecision;
	using namespace boost::multiprecision;

	template<typename... T>
	using vector = std::vector<T...>;

	template<typename... T>
	using array = std::array<T...>;

	using string = std::string;
	using time = fc::time_point;
	using signature = fc::ecc::compact_signature;
	using checksum = fc::sha256;
	using field_name = fc::fixed_string<>;
	using fixed_string32 = fc::fixed_string<fc::array<uint64_t, 4>>;// std::tuple<uint64_t,uint64_t,uint64_t,uint64_t>>;
	using fixed_string16 = fc::fixed_string<>;
	using type_name = fixed_string32;;
	using bytes = vector<char>;

	template<size_t Size>
	using uint_t = number<cpp_int_backend<Size, Size, unsigned_magnitude, unchecked, void> >;
	template<size_t Size>
	using int_t = number<cpp_int_backend<Size, Size, signed_magnitude, unchecked, void> >;

	using uint8 = uint_t<8>;
	using uint16 = uint_t<16>;
	using uint32 = uint_t<32>;
	using uint64 = uint_t<64>;
	using uint128 = boost::multiprecision::uint128_t;
	using uint256 = boost::multiprecision::uint256_t;
	using int8 = int8_t;//int_t<8>;  these types are different sizes than Native...
	using int16 = int16_t; //int_t<16>;
	using int32 = int32_t; //int_t<32>;
	using int64 = int64_t; //int_t<64>;
	using int128 = boost::multiprecision::int128_t;
	using int256 = boost::multiprecision::int256_t;
#if WIN32
	using uint128_t = _int128; /// native clang/gcc 128 intrinisic
#else
	using uint128_t = unsigned __int128;
#endif

}}