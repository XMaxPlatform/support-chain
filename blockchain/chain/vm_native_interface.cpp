/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>
#include <vm_native_interface_database.hpp>
#include <vm_native_interface_message.hpp>
#include <vm_native_interface_math.hpp>
#include <vm_native_interface_transaction.hpp>
#include <message_context_xmax.hpp>
#include <blockchain_exceptions.hpp>
#include <fc/uint128.hpp>

namespace Xmaxplatform {namespace Chain {

	const uint32_t bytes_per_mbyte = 1024 * 1024;
	using namespace vm_ds;

	void vm_native_log()
	{
		vm_database_log();
		vm_message_log();
		vm_math_log();
		vm_transaction_log();
	}

	void vm_checktime()
	{
		vm_xmax::get().checktime();
	}BIND_VM_NATIVE_FUCTION(vm_checktime, ds_void, checktime)

	void vm_prints(const std::string& str)
	{
		ilog(str);
	}BIND_VM_NATIVE_FUCTION_R1(vm_prints, ds_void, prints, ds_string)


	ds_time::vm_type vm_xmax_now()
	{
		return vm_xmax::get().current_validate_context->current_time().sec_since_epoch();
	}BIND_VM_NATIVE_FUCTION(vm_xmax_now, ds_time, xmax_now)

		
	void vm_xmax_assert(uint32_t test, const string& msg)
	{
		if (!test) edump((msg));
		FC_ASSERT(test, "assertion failed: ${s}", ("s", msg));
	}
	BIND_VM_NATIVE_FUCTION_R2(vm_xmax_assert, ds_void, xmax_assert, ds_int, ds_string)

	char char_to_symbol(char c)
	{
		if (c >= (char)'a' && c <= (char)'z')
			return (c - (char)'a') + 6;
		if (c >= '1' && c <= '5')
			return (c - '1') + 1;
		return 0;
	}

	int64 vm_string_to_name(const string& str)
	{
		uint32_t len = 0;
		while (str[len]) ++len;

		int64 value = 0;

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
	BIND_VM_NATIVE_FUCTION_R1(vm_string_to_name, ds_i64, str2n, ds_string)

	bool vm_xmax_namecom(int64 test,const string& name)
	{
		if (vm_string_to_name(name) == test)
		{
			return true;
		}
		return false;
	}
	BIND_VM_NATIVE_FUCTION_R2(vm_xmax_namecom, ds_bool, strcmpn,ds_i64, ds_string)

}}