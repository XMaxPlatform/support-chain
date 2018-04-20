#pragma once

// for win32 only. no uint128 in win32.
#ifdef WIN32

struct _uint128_win
{
public:
	unsigned long val[2];
};

#endif