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

}

#define XNAMETOSTR(NAME) ::xmax::name_to_string(NAME)