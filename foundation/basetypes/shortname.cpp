/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <shortname.hpp>

namespace ShortCode{

	short_name::short_name()
		: namecode_(0)
	{

	}

	short_name::short_name(name_code c)
	{
		set(c);
	}

	short_name::short_name(const char* str)
	{
		set(str);
	}

	short_name::short_name(const string& str)
	{
		set(str);
	}


	void short_name::set(name_code c)
	{
		namecode_ = c;
	}

	void short_name::set(const char* str)
	{
		namecode_ = to_name_code(str);
	}

	void short_name::set(const string& str)
	{
		namecode_ = to_name_code(str.c_str());
	}

	short_name short_name::create(name_code c)
	{
		return short_name(c);
	}

	string short_name::to_string() const
	{
		return short_name::to_string(namecode_);
	}
	string short_name::to_string(name_code code)
	{
		int32_t remains = NAME_CODE_BITS;
		string name;

		name_code namecode = code;
		name_code tmpcode = 0;
		while (remains >= SCODE_R_0_BITS)
		{
			tmpcode = ((name_code)namecode) & HEAD_MARK_R_0;

			if (tmpcode != 0)
			{
				// region 0
				remains -= SCODE_R_0_BITS;
				sglyph ch = get_sglyph(SR0_NUM, tmpcode >> (NAME_CODE_BITS - SCODE_R_0_BITS));
				name.push_back(ch);
				namecode = namecode << SCODE_R_0_BITS;
				continue;
			}

			if (remains < SCODE_R_1_BITS)
			{
				break;
			}
			tmpcode = ((name_code)namecode) & HEAD_MARK_R_1;
			if (tmpcode != 0)
			{
				// region 1
				remains -= SCODE_R_1_BITS;
				sglyph ch = get_sglyph(SR1_NUM, tmpcode >> (NAME_CODE_BITS - SCODE_R_1_BITS));
				name.push_back(ch);
				namecode = namecode << SCODE_R_1_BITS;
				continue;
			}
			// name end.
			break;
		}
		return name;
	}

	string short_name::to_string(short_name name)
	{
		return to_string(name.code());
	}

	uint32_t short_name::glyph_bit_length(sglyph gl)
	{
		auto desc = get_scode_rs(gl);

		switch (desc.region)
		{
		case SR0_NUM:
		{
			return SCODE_R_0_BITS;
		}
		case SR1_NUM:
		{
			return SCODE_R_1_BITS;
		}
		default:
			break;
		}

		return 0;
	}

	uint32_t short_name::name_length_with_bit(const char* str)
	{
		uint32_t len = 0;

		while (0 != *str)
		{
			len += glyph_bit_length(static_cast<sglyph>(*str));
			++str;
		}

		return len;
	}

	bool short_name::legal_name(const char* str)
	{

		uint32_t totallen = 0;

		while (0 != *str)
		{
			uint32_t len = glyph_bit_length(static_cast<sglyph>(*str));
			if (0 == len)
			{
				return false;
			}
			totallen += len;
			++str;
		}

		if (totallen > NAME_CODE_BITS)
		{
			return false;
		}

		return true;
	}
}