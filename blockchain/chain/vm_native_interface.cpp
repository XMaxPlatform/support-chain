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

	void vm_native_log()
	{
		vm_database_log();
		vm_message_log();
	}

	void vm_checktime()
	{
		vm_xmax::get().checktime();
	}BIND_VM_NATIVE_FUCTION_VOID(vm_checktime, checktime)

	void vm_prints(const std::string& str)
	{
		ilog(str);
	}BIND_VM_NATIVE_FUCTION_VOID_R(vm_prints, prints, vm_arg::ds_string)

		
//	vm_arg::time_t vm_xmax_now()
//	{
//		return vm_xmax::get().current_validate_context->current_time().sec_since_epoch();
//	}
//	BIND_VM_NATIVE_FUCTION_RT_VIOD(vm_xmax_now, xmax_now, vm_arg::ds_time)

	DEFINE_INTRINSIC_FUNCTION0(env, now, xmax_now, i32) {
		return vm_xmax::get().current_validate_context->current_time().sec_since_epoch();
	}

		/*
	void vm_xmax_assert(uint32_t test, const char* cstr)
	{
		//XMAX_ASSERT(test,cstr);
	}
	BIND_VM_NATIVE_FUCTION_VOID_R2(vm_xmax_assert, xmax_assert, vm_arg::ds_int, vm_arg::ds_string)*/
		
	DEFINE_INTRINSIC_FUNCTION2(env, assert, xmax_assert, none, i32, test, i32, msg) {
		const char* m = &Runtime::memoryRef<char>(vm_xmax::get().current_memory, msg);
		std::string message(m);
		if (!test) edump((message));
		FC_ASSERT(test, "assertion failed: ${s}", ("s", message)("ptr", msg));
	}


	DEFINE_INTRINSIC_FUNCTION2(evn, floatMinxx, floatMinxx, f32, f32, left, f32, right) 
	{ 
		return 0; 
	}

	void checktime(int64_t duration, uint32_t checktime_limit)
	{
		if (duration > checktime_limit) {
			wlog("checktime called ${d}", ("d", duration));
			throw checktime_exceeded();
		}
	}

	DEFINE_INTRINSIC_FUNCTION0(env, checktime, checktime, none) {
		checktime(vm_xmax::get().current_execution_time(), vm_xmax::get().checktime_limit);
	}
	/*

	//--------------------------------message------------------------------------------//
	DEFINE_INTRINSIC_FUNCTION0(env, current_code, current_code, i64) {
		auto& wasm = vm_xmax::get();
		return wasm.current_validate_context->code.value;
	}

	DEFINE_INTRINSIC_FUNCTION1(env, require_auth, require_auth, none, i64, account) {
		vm_xmax::get().current_validate_context->require_authorization(name(account));
	}

	DEFINE_INTRINSIC_FUNCTION1(env, require_notice, require_notice, none, i64, account) {
		vm_xmax::get().current_message_context->require_recipient(account);
	}


	DEFINE_INTRINSIC_FUNCTION0(env, message_size, message_size, i32) {
		return vm_xmax::get().current_validate_context->msg.data.size();
	}

	DEFINE_INTRINSIC_FUNCTION2(env, read_message, read_message, i32, i32, destptr, i32, destsize) {
		FC_ASSERT(destsize > 0);

		vm_xmax& wasm = vm_xmax::get();
		auto  mem = wasm.current_memory;
		char* begin = memoryArrayPtr<char>(mem, destptr, uint32_t(destsize));

		int minlen = std::min<int>(wasm.current_validate_context->msg.data.size(), destsize);

		memcpy(begin, wasm.current_validate_context->msg.data.data(), minlen);
		return minlen;
	}
	
	*/
}}