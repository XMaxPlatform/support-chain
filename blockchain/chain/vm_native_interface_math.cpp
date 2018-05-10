/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>


namespace Xmaxplatform {namespace Chain {

	void vm_math_log()
	{

	}

	DEFINE_INTRINSIC_FUNCTION2(env, multeq_i128, multeq_i128, none, i32, self, i32, other) {
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;
		auto& v = memoryRef<uint128>(mem, self);
		const auto& o = memoryRef<const uint128>(mem, other);
		v *= o;
	}

	DEFINE_INTRINSIC_FUNCTION2(env, diveq_i128, diveq_i128, none, i32, self, i32, other) {
		vm_xmax& wasm = vm_xmax::get();								\
			Runtime::MemoryInstance*   mem = wasm.current_memory;
		auto& v = memoryRef<uint128>(mem, self);
		const auto& o = memoryRef<const uint128>(mem, other);
		FC_ASSERT(o != 0, "divide by zero");
		v /= o;
	}
}}