/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>
#include <vm_native_interface_database.hpp>
#include <vm_native_interface_message.hpp>
#include <message_context_xmax.hpp>
#include <blockchain_exceptions.hpp>



namespace Xmaxplatform {namespace Chain {

	const uint32_t bytes_per_mbyte = 1024 * 1024;
	using namespace vm_ds;

	void vm_native_log()
	{
		vm_database_log();
		vm_message_log();
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
	



}}