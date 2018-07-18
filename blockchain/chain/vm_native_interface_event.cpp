/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>
#include <blockchain_exceptions.hpp>
#include <string_utilities.hpp>


namespace Xmaxplatform {namespace Chain {
	using namespace vm_ds;
	
	void vm_event_log()
	{

	}

	//--------------------------------event------------------------------------------//

	void fire_event_internal(uint128 event_name, int srcptr, int srcsize)
	{
		FC_ASSERT(srcsize > 0);

		auto &wasm = vm_xmax::get();
		auto& msg = wasm.current_validate_context->msg;
		
		wasm.current_message_context->events.push_back(event_output{ static_cast<Xmaxplatform::Basetypes::event_name>(event_name), 
			msg.code, msg.type, bytes(), XNAMETOSTR(event_name).c_str() });
		auto& event = wasm.current_message_context->events.back();
		event.data.resize(srcsize);
		const char* e = &Runtime::memoryRef<char>(wasm.current_memory, srcptr);
		memcpy(event.data.data(), e, srcsize);
	}
	BIND_VM_NATIVE_FUCTION_R3(fire_event_internal, ds_void, fire_event_internal, ds_u128, ds_i32, ds_i32)

}}