#pragma once
#include <string>


namespace xmax {
 
	const int symbol_alphabet_begin = 6;
	const int  symbol_alphabet_end = 31;
	const int  symbol_num_begin = 1;
	const int  symbol_num_end = 5;

#if WIN32
	static char symbol_to_char(char s)
#else
	static constexpr char symbol_to_char(char s)
#endif
	{
		if (s >= symbol_alphabet_begin && s <= symbol_alphabet_end)
			return s - (char)6 + 'a';

		if (s >= symbol_num_begin && s <= symbol_num_end)
			return s - (char)1 + '1';

		return 0;
	}

#if WIN32
	static std::string&& name_to_string(uint64_t name)
#else
	static constexpr std::string&& name_to_string(uint64_t name)
#endif
	{
		if (name == 0)
			return "";

		uint64_t tmp = name;
		std::string resStr = "";

		for (int i = 0; i < 12; ++i)
		{
			char symbol = (char)((tmp >> (64 - 5 * (i + 1))) & 0x1f);
			if (!symbol)
			{
				break;
			}
			resStr += symbol_to_char(symbol);
		}

		return std::move(resStr);
	}


#if WIN32
	static  char char_to_symbol(char c)
#else
	static constexpr char char_to_symbol(char c)
#endif
	{
		if (c >= (char)'a' && c <= (char)'z')
			return (c - (char)'a') + 6;
		if (c >= '1' && c <= '5')
			return (c - '1') + 1;
		return 0;
	}

#if WIN32
	static  uint64_t string_to_name(const char* str)
#else
	static constexpr uint64_t string_to_name(const char* str)
#endif
	{
		uint32_t len = 0;
		while (str[len]) ++len;

		uint64_t value = 0;

		for (uint32_t i = 0; i <= 12; ++i) {
			uint64_t c = 0;
			if (i < len && i <= 12) c = uint64_t(char_to_symbol(str[i]));

			if (i < 12) {
				c &= 0x1f;
				c <<= 64 - 5 * (i + 1);
			}
			else {
				c &= 0x0f;
			}

			value |= c;
		}

		return value;
	}
}

#define XNAME(NAME) ::xmax::string_to_name(#NAME)
#define XNAMETOSTR(NAME) ::xmax::name_to_string(NAME) // Must be re-implement in because of conflict of the type 'time' with CRT in xmaxlib