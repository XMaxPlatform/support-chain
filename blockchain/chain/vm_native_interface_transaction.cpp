/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <blockchain_exceptions.hpp>
#include <vm_native_interface.hpp>

namespace Xmaxplatform {namespace Chain {
	using namespace vm_ds;
	
	void vm_transaction_log()
	{

	}

	int vm_message_create(int64 code, int64 type, int data, int length)
	{
		auto& wasm = vm_xmax::get();
		auto  mem = wasm.current_memory;

		XMAX_ASSERT(length >= 0, tx_unknown_argument,
			"Pushing a message with a negative length");

		bytes payload;
		if (length > 0) {
			try {
				// memoryArrayPtr checks that the entire array of bytes is valid and
				// within the bounds of the memory segment so that transactions cannot pass
				// bad values in attempts to read improper memory
				const char* buffer = memoryArrayPtr<const char>(mem, uint32_t(data), uint32_t(length));
				payload.insert(payload.end(), buffer, buffer + length);
			}
			catch (const Runtime::Exception& e) {
				FC_THROW_EXCEPTION(tx_unknown_argument, "Message data is not a valid memory range");
			}
		}

		auto& pmsg = wasm.current_validate_context->create_pending_message(name(code), name(type), payload);
		return pmsg.handle;
	}
	BIND_VM_NATIVE_FUCTION_R4(vm_message_create, ds_int, message_create, ds_i64, ds_i64,ds_int,ds_int)


	void vm_message_require_permission(int handle, int64 account, int64 permission)
	{
		auto apply_context = vm_xmax::get().current_validate_context;
		// if this is not sent from the code account with the permission of "code" then we must
		// presently have the permission to add it, otherwise its a failure
		if (!(account == apply_context->code.value && name(permission) == name("code"))) {
			apply_context->require_authorization(name(account), name(permission));
		}
		auto& pmsg = apply_context->get_pending_message(handle);
		pmsg.authorization.emplace_back(name(account), name(permission));
	}
	BIND_VM_NATIVE_FUCTION_R3(vm_message_require_permission, ds_void, message_require_permission, ds_int, ds_i64, ds_i64)

	void vm_message_send(int handle)
	{
		auto apply_context = vm_xmax::get().current_validate_context;
		auto& pmsg = apply_context->get_pending_message(handle);

		apply_context->inline_messages.emplace_back(pmsg);
		apply_context->release_pending_message(handle);
	}
	BIND_VM_NATIVE_FUCTION_R1(vm_message_send, ds_void, message_send, ds_int)

	void vm_message_drop(int handle)
	{
		vm_xmax::get().current_validate_context->release_pending_message(handle);
	}
	BIND_VM_NATIVE_FUCTION_R1(vm_message_drop, ds_void, message_drop, ds_int)

}}