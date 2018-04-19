/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <vm_xmax.hpp>

#include "Platform/Platform.h"
#include "WAST/WAST.h"
#include "Runtime/Runtime.h"
#include "Runtime/Linker.h"
#include "Runtime/Intrinsics.h"
#include "IR/Module.h"
#include "IR/Operators.h"
#include "IR/Validate.h"

namespace Xmaxplatform {namespace Chain {

	using namespace IR;
	using namespace Runtime;

	void vm_native_log();
	void checktime(int64_t duration, uint32_t checktime_limit);
	namespace vm_arg
	{
		typedef Runtime::MemoryInstance meminst;
		typedef NativeTypes::i32 int_t;
		typedef uint32 time_t;
		typedef int_t ptr_t;

		typedef int_t value_t;

		static const IR::ValueType ir_int_t = ValueType::i32;
		static const IR::ValueType ir_ptr_t = ir_int_t;

		template<typename _Type, typename _VM_Type, IR::ValueType _IR_Type>
		class ds
		{
		public:
			typedef _Type value_type;
			typedef _VM_Type vm_type;
			typedef IR::ValueType ir_type;

			static ir_type get_ir()
			{
				return _IR_Type;
			}

		};

		template<typename _Type>
		class ds_ptr_base : public ds<_Type, ptr_t, ir_ptr_t>
		{
		public:
			typedef ds<_Type, ptr_t, ir_ptr_t> super;

			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::ir_type ir_type;
		};

		template<typename _Type>
		class ds_ptr : public ds_ptr_base<_Type>
		{
		public:
			typedef ds_ptr_base<_Type> super;

			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::ir_type ir_type;

			typedef value_type* ptr_type;

			ds_ptr(meminst* mem, vm_type val)
			{
				ptr = &memoryRef<value_type>(mem, val);
			}

			const ptr_type& get() const
			{
				return ptr;
			}

		private:
			ptr_type ptr;

		};

		class ds_string : public ds_ptr_base<std::string>
		{
		public:
			typedef ds_ptr_base<std::string> super;

			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::ir_type ir_type;

			ds_string(meminst* mem, vm_type val)
			{
				str = &Runtime::memoryRef<const char>(mem, val);
			}

			const value_type& get() const
			{
				return str;
			}

		private:
			std::string str;

		};

		template<typename _Type, typename _VM_Type, IR::ValueType _IR_Type>
		class ds_val : public ds<_Type, _VM_Type, _IR_Type>
		{
		public:
			typedef ds<_Type, ptr_t, _IR_Type> super;

			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::ir_type ir_type;

			ds_val(meminst* mem, vm_type _val)
			{
				val = (value_type)_val;
			}
			ds_val(vm_type _val)
			{
				val = (value_type)_val;
			}		
			ds_val(value_type _val)
			{
				val = _val;
			}
			ds_val()
			{
				val = value_type(0);
			}
			const value_type& get() const
			{
				return val;
			}
			const vm_type& get_vm() const
			{
				return (vm_type)val;
			}

			static vm_type to_vm(value_type _val)
			{
				return (vm_type)_val;
			}

			value_type val;
		};



		typedef ds_val<size_t, int_t, ir_int_t> ds_int; // ds for size_t
		typedef ds_val<int32, NativeTypes::i32, ValueType::i32> ds_i32; // ds for int32
		typedef ds_val<int64, NativeTypes::i64, ValueType::i64> ds_i64; // ds for int64

		typedef ds_val<float, NativeTypes::f32, ValueType::f32> ds_f32; // ds for float
		typedef ds_val<double, NativeTypes::f64, ValueType::f64> ds_f64; // ds for double

		typedef ds_val<bool, int_t, ir_int_t> ds_bool; // ds for bool
		typedef ds_val<uint32, uint32, ValueType::i32> ds_time; // ds for bool

		typedef ds_ptr<const char> ds_cp_char; // ds for const char *
	}

}}





#define BIND_VM_NATIVE_FUCTION_VOID_R(nativ_func, vm_func) \
	void f_vm_void##vm_func##r_type(); \
	static Intrinsics::Function s_vm_void##vm_func##r_type	\
	("env" "." #vm_func, \
		IR::FunctionType::get(IR::ResultType::none), \
	(void*)&f_vm_void##vm_func##r_type);\
	void f_vm_void##vm_func##r_type() { \
		##nativ_func();\
	}

#define BIND_VM_NATIVE_FUCTION_VOID_R(nativ_func, vm_func, arg_ds1) \
	void f_vm_void##vm_func##r_type(arg_ds1::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func##r_type	\
	("env" "." #vm_func, \
		IR::FunctionType::get(IR::ResultType::none, { arg_ds1::get_ir() }), \
	(void*)&f_vm_void##vm_func##r_type);\
	void f_vm_void##vm_func##r_type(arg_ds1::vm_type val1) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		##nativ_func(arg_ds1(mem, val1).get());\
	}

#define BIND_VM_NATIVE_FUCTION_VOID_R2(nativ_func, vm_func, arg_ds1, arg_ds2) \
	void f_vm_void##vm_func##r_type(arg_ds1::vm_type, arg_ds2::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func##r_type	\
	("env" "." #vm_func, \
		IR::FunctionType::get(IR::ResultType::none, { arg_ds1::get_ir(), arg_ds2::get_ir() }), \
	(void*)&f_vm_void##vm_func##r_type);\
	void f_vm_void##vm_func##r_type(arg_ds1::vm_type val1, arg_ds2::vm_type val2) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		##nativ_func(arg_ds1(mem, val1).get(), arg_ds2(mem, val2).get());\
	}


#define BIND_VM_NATIVE_FUCTION_RT_VIOD(nativ_func, vm_func, rt) \
	rt::vm_type f_vm_rt##vm_func##r_type(); \
	static Intrinsics::Function s_vm_void##vm_func##r_type	\
	("env" "." #vm_func, \
		IR::FunctionType::get(IR::ResultType::none), \
	(void*)&f_vm_rt##vm_func##r_type);\
	rt::vm_type f_vm_rt##vm_func##r_type() { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return rt::to_vm( ##nativ_func() );\
	}

#define BIND_VM_NATIVE_FUCTION_RT_R(nativ_func, vm_func, rt, arg_ds) \
	rt::vm_type f_vm_rt##vm_func##r_type(arg_ds::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func##r_type	\
	("env" "." #vm_func, \
		IR::FunctionType::get(IR::ResultType::none, { arg_ds::get_ir() }), \
	(void*)&f_vm_void##vm_func##r_type);\
	rt::vm_type f_vm_void##vm_func##r_type(arg_ds::vm_type val1) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return rt::to_vm( ##nativ_func(arg_ds(mem, val1).get()) );\
	}
