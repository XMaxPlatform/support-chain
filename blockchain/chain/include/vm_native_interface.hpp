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

	namespace vm_arg
	{
		typedef Runtime::MemoryInstance meminst;
		typedef NativeTypes::i32 vm_int_t;
		typedef uint32 time_t;
		typedef vm_int_t ptr_t;

		typedef vm_int_t value_t;

		static const IR::ValueType vm_value_t = ValueType::i32;
		static const IR::ResultType vm_result_t = IR::ResultType::i32;

		static const IR::ValueType vm_ptr_t = vm_value_t;


		template<typename _Type, typename _VM_Type, IR::ValueType _VM_V_Type, IR::ResultType _VM_Result_Type>
		class ds
		{
		public:
			typedef _Type value_type;
			typedef _VM_Type vm_type;
			typedef IR::ValueType vm_value_type;
			typedef IR::ResultType vm_result_type;

			static inline vm_value_type get_vm_value_type()
			{
				return _VM_V_Type;
			}
			static inline vm_result_type get_vm_result_type()
			{
				return _VM_Result_Type;
			}

		};

		class ds_void
		{
		public:
			typedef void value_type;
			typedef NativeTypes::none vm_type;
			typedef IR::ValueType vm_value_type;
			typedef IR::ResultType vm_result_type;

			static inline vm_value_type get_vm_value_type()
			{
				return IR::ValueType::any;
			}
			static inline vm_result_type get_vm_result_type()
			{
				return IR::ResultType::none;
			}
		};

		template<typename _Type>
		class ds_ptr_base : public ds<_Type, ptr_t, vm_ptr_t, vm_result_t>
		{
		public:
			typedef ds<_Type, ptr_t, vm_ptr_t, vm_result_t> super;

			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::vm_value_type vm_value_type;
			typedef typename super::vm_result_type vm_result_type;
		};

		template<typename _Type>
		class ds_ptr : public ds_ptr_base<_Type>
		{
		public:
			typedef ds_ptr_base<_Type> super;
			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::vm_value_type vm_value_type;
			typedef typename super::vm_result_type vm_result_type;

			typedef value_type* ptr_type;

			ds_ptr(meminst* mem, vm_type val)
			{
				ptr = &memoryRef<value_type>(mem, val);
			}

			inline const ptr_type& get() const
			{
				return ptr;
			}

		private:
			ptr_type ptr;

		};

		class ds_char_string : public ds_ptr_base<std::string>
		{
		public:
			typedef ds_ptr_base<std::string> super;
			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::vm_value_type vm_value_type;
			typedef typename super::vm_result_type vm_result_type;

			ds_char_string(meminst* mem, vm_type val)
			{
				str = &Runtime::memoryRef<const char>(mem, val);
			}

			inline const value_type& get() const
			{
				return str;
			}

		private:
			std::string str;

		};

		template<typename _Type, typename _VM_Type, IR::ValueType _VM_V_Type, IR::ResultType _VM_Result_Type>
		class ds_val : public ds<_Type, _VM_Type, _VM_V_Type, _VM_Result_Type>
		{
		public:
			typedef ds<_Type, ptr_t, _VM_V_Type, _VM_Result_Type> super;

			typedef typename super::value_type value_type;
			typedef typename super::vm_type vm_type;
			typedef typename super::vm_value_type vm_value_type;
			typedef typename super::vm_result_type vm_result_type;

			ds_val(meminst* mem, vm_type _val)
			{
				val = (value_type)_val;
			}
			ds_val(vm_type _val)
			{
				val = (value_type)_val;
			}		

			ds_val()
			{
				val = value_type(0);
			}
			inline const value_type& get() const
			{
				return val;
			}
			inline const vm_type& get_vm() const
			{
				return (vm_type)val;
			}

			static inline vm_type to_vm(value_type _val)
			{
				return (vm_type)_val;
			}

			value_type val;
		};


	}
	namespace vm_ds
	{

		typedef vm_arg::ds_val<int32, vm_arg::vm_int_t, vm_arg::vm_value_t, vm_arg::vm_result_t> ds_int; // ds for size_t
		typedef vm_arg::ds_val<int32, NativeTypes::i32, ValueType::i32, ResultType::i32> ds_i32; // ds for int32
		typedef vm_arg::ds_val<int64, NativeTypes::i64, ValueType::i64, ResultType::i64> ds_i64; // ds for int64

		typedef vm_arg::ds_val<float, NativeTypes::f32, ValueType::f32, ResultType::f32> ds_f32; // ds for float
		typedef vm_arg::ds_val<double, NativeTypes::f64, ValueType::f64, ResultType::f64> ds_f64; // ds for double

		typedef vm_arg::ds_val<bool, vm_arg::vm_int_t, vm_arg::vm_value_t, vm_arg::vm_result_t> ds_bool; // ds for bool
		typedef vm_arg::ds_val<uint32, uint32, ValueType::i32, ResultType::i32> ds_time; // ds for bool

		typedef vm_arg::ds_ptr<const char> ds_cp_char; // ds for const char *
		typedef vm_arg::ds_char_string ds_string; // ds for string	
		typedef vm_arg::ds_void	ds_void;
	}



}}

#define VM_NATIVE_FUCTION_NAME(vm_func) f_vm_##vm_func

#define BIND_VM_NATIVE_FUCTION(nativ_func, vm_rt, vm_func) \
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(); \
	static Intrinsics::Function s_vm_void##vm_func	\
	("env" "." #vm_func, \
		IR::FunctionType::get(vm_rt::get_vm_result_type()), \
	(void*)&VM_NATIVE_FUCTION_NAME(vm_func));\
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)() { \
	 return ( ##nativ_func() );\
	}

#define BIND_VM_NATIVE_FUCTION_R1(nativ_func, vm_rt, vm_func, arg_ds) \
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func	\
	("env" "." #vm_func, \
		IR::FunctionType::get(vm_rt::get_vm_result_type(), { arg_ds::get_vm_value_type() }), \
	(void*)&VM_NATIVE_FUCTION_NAME(vm_func));\
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds::vm_type val1) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return ( ##nativ_func(arg_ds(mem, val1).get()) );\
	}

#define BIND_VM_NATIVE_FUCTION_R2(nativ_func, vm_rt, vm_func, arg_ds1, arg_ds2) \
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type, arg_ds2::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func	\
	("env" "." #vm_func, \
		IR::FunctionType::get(vm_rt::get_vm_result_type(), { arg_ds1::get_vm_value_type(), arg_ds2::get_vm_value_type() }), \
	(void*)&VM_NATIVE_FUCTION_NAME(vm_func));\
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type val1, arg_ds2::vm_type val2) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return ( ##nativ_func(arg_ds1(mem, val1).get(), arg_ds2(mem, val2).get()) );\
	}

#define BIND_VM_NATIVE_FUCTION_R3(nativ_func, vm_rt, vm_func, arg_ds1, arg_ds2, arg_ds3) \
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type, arg_ds2::vm_type, arg_ds3::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func	\
	("env" "." #vm_func, \
		IR::FunctionType::get(vm_rt::get_vm_result_type(), { arg_ds1::get_vm_value_type(), arg_ds2::get_vm_value_type(), arg_ds3::get_vm_value_type() }), \
	(void*)&VM_NATIVE_FUCTION_NAME(vm_func));\
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type val1, arg_ds2::vm_type val2, arg_ds3::vm_type val3) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return ( ##nativ_func( arg_ds1(mem, val1).get(), arg_ds2(mem, val2).get(), arg_ds3(mem, val3).get() ) );\
	}

#define BIND_VM_NATIVE_FUCTION_R4(nativ_func, vm_rt, vm_func, arg_ds1, arg_ds2, arg_ds3, arg_ds4) \
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type, arg_ds2::vm_type, arg_ds3::vm_type, arg_ds4::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func	\
	("env" "." #vm_func, \
		IR::FunctionType::get(vm_rt::get_vm_result_type(), { arg_ds1::get_vm_value_type(), arg_ds2::get_vm_value_type(), arg_ds3::get_vm_value_type(), arg_ds4::get_vm_value_type() }), \
	(void*)&VM_NATIVE_FUCTION_NAME(vm_func));\
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type val1, arg_ds2::vm_type val2, arg_ds3::vm_type val3, arg_ds4::vm_type val4) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return ( ##nativ_func( arg_ds1(mem, val1).get(), arg_ds2(mem, val2).get(), arg_ds3(mem, val3).get(), arg_ds4(mem, val4).get() ) );\
	}

#define BIND_VM_NATIVE_FUCTION_R4(nativ_func, vm_rt, vm_func, arg_ds1, arg_ds2, arg_ds3, arg_ds4, arg_ds5) \
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type, arg_ds2::vm_type, arg_ds3::vm_type, arg_ds4::vm_type, arg_ds5::vm_type); \
	static Intrinsics::Function s_vm_void##vm_func	\
	("env" "." #vm_func, \
		IR::FunctionType::get(vm_rt::get_vm_result_type(), { arg_ds1::get_vm_value_type(), arg_ds2::get_vm_value_type(), arg_ds3::get_vm_value_type(), arg_ds4::get_vm_value_type(), arg_ds5::get_vm_value_type() }), \
	(void*)&VM_NATIVE_FUCTION_NAME(vm_func));\
	vm_rt::vm_type VM_NATIVE_FUCTION_NAME(vm_func)(arg_ds1::vm_type val1, arg_ds2::vm_type val2, arg_ds3::vm_type val3, arg_ds4::vm_type val4, arg_ds5::vm_type val5) { \
		vm_xmax& wasm = vm_xmax::get();								\
		Runtime::MemoryInstance*   mem = wasm.current_memory;		\
		return ( ##nativ_func( arg_ds1(mem, val1).get(), arg_ds2(mem, val2).get(), arg_ds3(mem, val3).get(), arg_ds4(mem, val4).get(), arg_ds5(mem, val5).get() ) );\
	}

