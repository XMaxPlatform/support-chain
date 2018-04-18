/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>

namespace Xmaxplatform {namespace Chain {



	void vm_prints(const std::string& str)
	{
		ilog(str);
	}
	BIND_VM_NATIVE_FUCTION_VOID_R(vm_prints, prints, vm_arg::ds_string)


	vm_arg::time_t vm_xmax_now()
	{
		return vm_xmax::get().current_validate_context->current_time().sec_since_epoch();
	}
	BIND_VM_NATIVE_FUCTION_RT_VIOD(vm_xmax_now, xmax_now, vm_arg::ds_time)
}}