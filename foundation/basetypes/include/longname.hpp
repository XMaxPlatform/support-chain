/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <basedef.hpp>
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
		
		static inline std::string to_string(NameCodeType name) {
			static_assert(false, "Not implemented yet.");
			return "";
		}

		static inline NameCodeType to_name_code(const char* str) {
			static_assert(false, "Not implemented yet.");
			return NameCodeType{};
		}		
	
		NameCodeType namecode_;
	};

	// Type specialization	

	//============= uint128 =================
	_CONST_EXPR_ std::string to_string_uint128(uint128 name_code);
	_CONST_EXPR_ uint128 uint128_to_name_code(const char* str);
	
	template<>
	static inline std::string long_name<uint128>::to_string(uint128 name) {
		return to_string_uint128(name);
	}

	template<>
	static inline uint128 long_name<uint128>::to_name_code(const char* str) {
		return uint128_to_name_code(str);
	}

	/*template<>
	class long_name<uint128> {
	public:


		static inline uint128 to_name_code(const char* str) {
			return uint128_to_name_code(str);
		}
	};*/
}