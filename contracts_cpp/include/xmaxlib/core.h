#pragma once
#include <xmaxlib/types.h>

extern "C" {
	void xmax_assert(uint32_t test, const char* cstr);

	[[noreturn]] void xmax_exit(int32_t code);

	time xmax_now();

	void prints(const char* cstr);

	bool strcmpn(int64_t test, const char* cstr);
	
}