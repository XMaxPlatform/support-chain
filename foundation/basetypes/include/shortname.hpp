/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <shortcode.hpp>

namespace ShortCode{

	using name_code = uint64_t;
	using std::string;

	static constexpr int32_t NAME_CODE_BITS = sizeof(name_code) * 8;
	static constexpr int32_t SCODE_R_0_BITS = 5;
	static constexpr int32_t SCODE_R_1_BITS = 8;

	static constexpr name_code HEAD_MARK_R_0 = ((name_code)0x1f) << (NAME_CODE_BITS - SCODE_R_0_BITS);
	static constexpr name_code HEAD_MARK_R_1 = ((name_code)0x07) << (NAME_CODE_BITS - SCODE_R_1_BITS);

	struct short_name
	{
	public:
		typedef name_code name_code_type;

		bool valid()const { return true; }
		bool empty()const { return 0 == namecode_; }
		bool good()const { return !empty() && valid(); }

		short_name();
		short_name(name_code c);
		short_name(const char* str);
		short_name(const string& str);
		inline name_code code() const
		{
			return namecode_;
		}

		friend bool operator < (const short_name& a, const short_name& b) { return a.namecode_ < b.namecode_; }
		friend bool operator <= (const short_name& a, const short_name& b) { return a.namecode_ <= b.namecode_; }
		friend bool operator > (const short_name& a, const short_name& b) { return a.namecode_ > b.namecode_; }
		friend bool operator >=(const short_name& a, const short_name& b) { return a.namecode_ >= b.namecode_; }
		friend bool operator == (const short_name& a, const short_name& b) { return a.namecode_ == b.namecode_; }

		friend bool operator == (const short_name& a, name_code b) { return a.namecode_ == b; }
		friend bool operator != (const short_name& a, name_code b) { return a.namecode_ != b; }

		friend bool operator != (const short_name& a, const short_name& b) { return a.namecode_ != b.namecode_; }

		template<typename Stream>
		friend Stream& operator << (Stream& out, const short_name& n) {
			return out << string(n);
		}

		short_name& operator=(name_code n) {
			set(n);
			return *this;
		}

		short_name& operator=(const string& n) {
			set(n);
			return *this;
		}
		short_name& operator=(const char* n) {
			set(n);
			return *this;
		}

		string to_string() const;

		static short_name create(name_code c);

		static string to_string(name_code code);

		static string to_string(short_name name);

		static uint32_t glyph_bit_length(sglyph gl);

		static uint32_t name_length_with_bit(const char* str);

		static bool legal_name(const char* str);

		static _CONST_EXPR_ name_code to_name_code(const char* str)
		{
			name_code namecode = 0;
			const char* ptr = str;
			int32_t remains = NAME_CODE_BITS;
			while (*ptr != 0)
			{
				char ch = *ptr;
				scode_rs rs = get_scode_rs(ch);
				if (SR0_NUM == rs.region)
				{
					if (remains < SCODE_R_0_BITS)
					{
						break;
					}
					remains -= SCODE_R_0_BITS;
					name_code mark = ((name_code)rs.symbol) << remains;
					namecode = (namecode | mark);
				}
				else if (SR1_NUM == rs.region)
				{
					if (remains < SCODE_R_1_BITS)
					{
						break;
					}
					remains -= SCODE_R_1_BITS;
					name_code mark = ((name_code)rs.symbol) << remains;
					namecode = (namecode | mark);
				}
				else
				{
					// illegal char.
				}
				++ptr;
			}
			return namecode;
		}
		static name_code to_name_code(const std::string& str)
		{
			return to_name_code(str.c_str());
		}

		void set(name_code c);
		void set(const char* str);
		void set(const string& str);

		operator bool()const { return namecode_ != 0; }
		operator name_code()const { return namecode_; }
		explicit operator string()const
		{
			return to_string();
		}
		name_code namecode_;
	};

}