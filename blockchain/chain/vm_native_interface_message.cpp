/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>
#include <message_context_xmax.hpp>
#include <blockchain_exceptions.hpp>

namespace Xmaxplatform {namespace Chain {
	using namespace vm_ds;
	
	void vm_message_log()
	{

	}

	//--------------------------------message------------------------------------------//

	int64 vm_xmax_current_code()
	{
		auto& wasm = vm_xmax::get();
		return wasm.current_validate_context->code.code();
	}
	BIND_VM_NATIVE_FUCTION(vm_xmax_current_code, ds_i64, current_code)

	void vm_xmax_require_auth(const std::string& account)
	{
		vm_xmax::get().current_validate_context->require_authorization(name(account));
	}
	BIND_VM_NATIVE_FUCTION_R1(vm_xmax_require_auth, ds_void, require_auth,ds_string)

	void vm_xmax_require_notice(const std::string& account)
	{
		vm_xmax::get().current_message_context->require_recipient(name(account));
	}
	BIND_VM_NATIVE_FUCTION_R1(vm_xmax_require_notice, ds_void, require_notice, ds_string)

	int32 vm_xmax_message_size()
	{
		return  vm_xmax::get().current_validate_context->msg.data.size();
	}
	BIND_VM_NATIVE_FUCTION(vm_xmax_message_size, ds_int, message_size)

	int vm_xmax_read_message(int destptr,int destsize)
	{
		FC_ASSERT(destsize > 0);

		vm_xmax& wasm = vm_xmax::get();
		auto  mem = wasm.current_memory;
		char* begin = memoryArrayPtr<char>(mem, destptr, uint32_t(destsize));

		int minlen = std::min<int>(wasm.current_validate_context->msg.data.size(), destsize);

		memcpy(begin, wasm.current_validate_context->msg.data.data(), minlen);
		return minlen;
	}
	BIND_VM_NATIVE_FUCTION_R2(vm_xmax_read_message, ds_int, read_message, ds_int,ds_int)

}}