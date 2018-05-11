#pragma once
#include <xmaxlib/types.h>


namespace xmax {

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