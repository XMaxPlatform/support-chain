/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <xmaxlib/types.h>

extern "C" {
	void xmax_assert(uint32_t test, const char* cstr);

	/**
	* This method will abort execution of wasm without failing the contract. This
	* is used to bypass all cleanup / destructors that would normally be called.
	*/
	[[noreturn]] void xmax_exit(int32_t code);

	/**
	*  Returns the time in seconds from 1970 of the last accepted block (not the block including this action)
	*  @brief Get time of the last accepted block
	*  @return time in seconds from 1970 of the last accepted block
	*/
	time xmax_now();


	/**
	*  Prints string
	*  @brief Prints string
	*  @param cstr - a null terminated string
	*
	*  Example:
	*  @code
	*  prints("Hello World!"); // Output: Hello World!
	*  @endcode
	*/
	void prints(const char* cstr);

}