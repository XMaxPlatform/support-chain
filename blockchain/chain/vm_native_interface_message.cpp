/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>
#include <message_context_xmax.hpp>
#include <blockchain_exceptions.hpp>

namespace Xmaxplatform {namespace Chain {

	
	void vm_message_log()
	{

	}

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


}}