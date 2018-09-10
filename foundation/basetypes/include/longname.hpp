/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <basedef.hpp>
#include <unordered_map>
#include <string>

#if WIN32
#	if _MSC_VER < 1914
#		define _CONST_EXPR_ 
#	else
#		define _CONST_EXPR_ constexpr
#	endif
#else
#		define _CONST_EXPR_ constexpr
#endif

namespace LongCode{
	using Xmaxplatform::Basetypes::uint128;
	//using std::string;
	
	template <class NameCodeType>
	class long_name
	{		
	public:		
		typedef NameCodeType name_code_type;
		typedef size_t size_type;
		typedef int8_t char_code_type;

		long_name() : namecode_() {}
		
		long_name(NameCodeType c): namecode_(c) {}
		
		long_name(const char* str) {
			namecode_ = to_name_code(str);
		}
		
		long_name(const std::string& str) {
			namecode_ = to_name_code(str.c_str());
		}

		long_name& operator=(NameCodeType n) {
			namecode_ = n;
			return *this;
		}

		long_name& operator=(const std::string& n) {
			namecode_ = to_name_code(n.c_str());
			return *this;
		}
		long_name& operator=(const char* n) {
			namecode_ = to_name_code(n);
			return *this;
		}

		inline NameCodeType code() const
		{
			return namecode_;
		}

		bool valid()const { return true; }
		bool empty()const { return 0 == namecode_; }
		bool good()const { return !empty() && valid(); }

		std::string to_string() const {
			return to_string(namecode_);
		}

		explicit operator std::string()const
		{
			return to_string();
		}

		friend bool operator < (const long_name& a, const long_name& b) { return a.namecode_ < b.namecode_; }
		friend bool operator <= (const long_name& a, const long_name& b) { return a.namecode_ <= b.namecode_; }
		friend bool operator > (const long_name& a, const long_name& b) { return a.namecode_ > b.namecode_; }
		friend bool operator >=(const long_name& a, const long_name& b) { return a.namecode_ >= b.namecode_; }
		friend bool operator == (const long_name& a, const long_name& b) { return a.namecode_ == b.namecode_; }

		friend bool operator == (const long_name& a, NameCodeType b) { return a.namecode_ == b; }
		friend bool operator != (const long_name& a, NameCodeType b) { return a.namecode_ != b; }

		friend bool operator != (const long_name& a, const long_name& b) { return a.namecode_ != b.namecode_; }

		template<typename Stream>
		friend Stream& operator << (Stream& out, const long_name& n) {
			return out << std::string(n);
		}

		operator bool()const { return namecode_ != 0; }
		operator NameCodeType()const { return namecode_; }
						

		static inline constexpr size_type char_bits() {
			static_assert(false, "Need implemented in the specific class.");
		}

		static inline constexpr size_type type_size() {
			return sizeof(NameCodeType);
		}

		static inline constexpr size_type type_bits() {
			return type_size() * 8;
		}

		static inline constexpr size_type max_char_count() {
			return type_bits() / char_bits();
		}

		static inline constexpr char_code_type char_code_mask() {
			return static_cast<char_code_type>((1 << char_bits()) - 1);
		}

		static inline constexpr char_code_type last_code_mask() {
			return static_cast<char_code_type>((1 << (type_bits() - max_char_count() * char_bits())) - 1);
		}
		
		static inline constexpr char_code_type invalid_code() {
			return 0;
		}

		static char_code_type char_to_code(char chr) {
			static_assert(false, "Need implemented in the specific class.");
		}

		static char code_to_char(char_code_type code) {
			static_assert(false, "Need implemented in the specific class.");
		}

		static inline std::string to_string(NameCodeType name_code) {
			if (name_code == invalid_code())
				return "";

			NameCodeType tmp = name_code;
			std::string res = "";

			for (size_type i = 0; i < max_char_count(); ++i)
			{
				char_code_type code = (char)((tmp >> (type_bits() - char_bits() * (i + 1))) & char_code_mask());
				if (code == invalid_code())
				{
					break;
				}
				res += code_to_char(code);
			}

			return res;
		}

		static inline NameCodeType to_name_code(const char* str) {
			size_type len = 0;
			while (str[len]) ++len;

			NameCodeType value = invalid_code();

			for (size_type i = 0; i <= max_char_count(); ++i) {
				NameCodeType c = 0;
				if (i < len && i <= max_char_count()) c = NameCodeType(char_to_code(str[i]));

				if (i < max_char_count()) {
					c &= char_code_mask();
					c <<= type_bits() - char_bits() * (i + 1);
				}
				else {
					c &= last_code_mask();
				}

				value |= c;
			}

			return value;
		}

	
		NameCodeType namecode_;
	};

	// Type specialization	

	// ---------------- uint128 -----------------------
	template<>
	inline constexpr long_name<uint128>::size_type long_name<uint128>::char_bits() { return 6; }

	template<>
	inline constexpr long_name<uint128>::size_type long_name<uint128>::type_size() { return 16; }


	typedef std::unordered_map<char, long_name<uint128>::char_code_type> reverse_code_map_type_u128;

	//  Construction of 128 bits: c0 c1 c2 ... c20 00
	//  Each char take 6 bits	
	const static char CODE_MAP_U128[] = {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'.', '#', '@', '$', '%', '_' , '-'
	};

	static inline reverse_code_map_type_u128 consturct_reverse_code_map_u128() {
		reverse_code_map_type_u128 reverse_code_map;
		for (long_name<uint128>::char_code_type i = 0; i < sizeof(CODE_MAP_U128); ++i)
		{
			reverse_code_map[CODE_MAP_U128[i]] = i + 1;
		}
		return reverse_code_map;
	}

	template<>
	static long_name<uint128>::char_code_type long_name<uint128>::char_to_code(char chr) {
		static reverse_code_map_type_u128 reverse_code_map = consturct_reverse_code_map_u128();
		return reverse_code_map.at(chr);
	}

	template<>
	static char long_name<uint128>::code_to_char(long_name<uint128>::char_code_type code) {
		static_assert((1 << char_bits()) > sizeof(CODE_MAP_U128), "The code map for type uint128 is too large to fit all characters in.");
		assert(code >= 0 && code < sizeof(CODE_MAP_U128));
		return CODE_MAP_U128[code - 1];
	}


}