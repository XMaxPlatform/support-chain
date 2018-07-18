/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <vm_native_interface.hpp>
#include <vm_native_interface_database.hpp>
#include <message_context_xmax.hpp>
#include <blockchain_exceptions.hpp>

namespace Xmaxplatform {namespace Chain {

	const uint32_t bytes_per_mbyte = 1024 * 1024;

	void vm_database_log()
	{

	}

	//----------------------------------------------------database---------------------------------------------//

	template <typename Function, typename KeyType, int numberOfKeys>
	int32_t validate(int32_t valueptr, int32_t valuelen, Function func) {

		static const uint32_t keylen = numberOfKeys * sizeof(KeyType);

		FC_ASSERT(valuelen >= keylen, "insufficient data passed");

		auto& wasm = vm_xmax::get();
		FC_ASSERT(wasm.current_message_context, "no apply context found");

		char* value = memoryArrayPtr<char>(wasm.current_memory, valueptr, valuelen);
		KeyType*  keys = reinterpret_cast<KeyType*>(value);

		valuelen -= keylen;
		value += keylen;

		return func(wasm.current_message_context, keys, value, valuelen);
	}

	template <typename Function>
	int32_t validate_str(int32_t keyptr, int32_t keylen, int32_t valueptr, int32_t valuelen, Function func) {

		auto& wasm = vm_xmax::get();
		FC_ASSERT(wasm.current_message_context, "no apply context found");

		char* key = memoryArrayPtr<char>(wasm.current_memory, keyptr, keylen);
		char* value = memoryArrayPtr<char>(wasm.current_memory, valueptr, valuelen);

		std::string keys(key, keylen);

		return func(wasm.current_message_context, &keys, value, valuelen);
	}

	int64_t round_to_byte_boundary(int64_t bytes)
	{
		const unsigned int byte_boundary = sizeof(uint64_t);
		int64_t remainder = bytes % byte_boundary;
		if (remainder > 0)
			bytes += byte_boundary - remainder;
		return bytes;
	}

#define VERIFY_TABLE(TYPE) \
   const auto table_name = name(table); \
   auto& wasm  = vm_xmax::get(); \
   if (wasm.table_key_types) \
   { \
      auto table_key = wasm.table_key_types->find(table_name); \
      if (table_key == wasm.table_key_types->end()) \
      { \
         FC_ASSERT(!wasm.tables_fixed, "abi did not define table ${t}", ("t", table_name)); \
         wasm.table_key_types->emplace(std::make_pair(table_name,vm_xmax::TYPE)); \
      } \
      else \
      { \
         FC_ASSERT(vm_xmax::TYPE == table_key->second, "abi definition for ${table} expects \"${type}\", but code is requesting \"" #TYPE "\"", ("table",table_name)("type",vm_xmax::to_type_name(table_key->second))); \
      } \
   }

#define READ_RECORD(READFUNC, INDEX, SCOPE) \
   auto lambda = [&](message_context_xmax* ctx, INDEX::value_type::key_type* keys, char *data, uint32_t datalen) -> int32_t { \
      auto res = ctx->READFUNC<INDEX, SCOPE>( name(scope), name(code), table_name, keys, data, datalen); \
      if (res >= 0) res += INDEX::value_type::number_of_keys*sizeof(INDEX::value_type::key_type); \
      return res; \
   }; \
   return validate<decltype(lambda), INDEX::value_type::key_type, INDEX::value_type::number_of_keys>(valueptr, valuelen, lambda);

#define UPDATE_RECORD(UPDATEFUNC, INDEX, DATASIZE) \
   auto lambda = [&](message_context_xmax* ctx, INDEX::value_type::key_type* keys, char *data, uint32_t datalen) -> int32_t { \
      return ctx->UPDATEFUNC<INDEX::value_type>( name(scope), name(ctx->code.code()), table_name, keys, data, datalen); \
   }; \
   const int32_t ret = validate<decltype(lambda), INDEX::value_type::key_type, INDEX::value_type::number_of_keys>(valueptr, DATASIZE, lambda);

#define DEFINE_RECORD_UPDATE_FUNCTIONS(OBJTYPE, INDEX, KEY_SIZE) \
   DEFINE_INTRINSIC_FUNCTION4(env,store_##OBJTYPE,store_##OBJTYPE,i32,u128,scope,u128,table,i32,valueptr,i32,valuelen) { \
      VERIFY_TABLE(OBJTYPE) \
      UPDATE_RECORD(store_record, INDEX, valuelen); \
      /* ret is -1 if record created, or else it is the length of the data portion of the originally stored */ \
      /* structure (it does not include the key portion */ \
      const bool created = (ret == -1); \
      int64_t& storage = vm_xmax::get().table_storage; \
      if (created) \
         storage += round_to_byte_boundary(valuelen) + vm_xmax::get().row_overhead_db_limit_bytes; \
      else \
         /* need to calculate the difference between the original rounded byte size and the new rounded byte size */ \
         storage += round_to_byte_boundary(valuelen) - round_to_byte_boundary(KEY_SIZE + ret); \
\
      XMAX_ASSERT(storage <= (vm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte), \
                 tx_code_db_limit_exceeded, \
                 "Database limit exceeded for account=${name}",("name", name(vm_xmax::get().current_message_context->code.code()))); \
      return created ? 1 : 0; \
   } \
   DEFINE_INTRINSIC_FUNCTION4(env,update_##OBJTYPE,update_##OBJTYPE,i32,u128,scope,u128,table,i32,valueptr,i32,valuelen) { \
      VERIFY_TABLE(OBJTYPE) \
      UPDATE_RECORD(update_record, INDEX, valuelen); \
      /* ret is -1 if record created, or else it is the length of the data portion of the originally stored */ \
      /* structure (it does not include the key portion */ \
      if (ret == -1) return 0; \
      int64_t& storage = vm_xmax::get().table_storage; \
      /* need to calculate the difference between the original rounded byte size and the new rounded byte size */ \
      storage += round_to_byte_boundary(valuelen) - round_to_byte_boundary(KEY_SIZE + ret); \
      \
      XMAX_ASSERT(storage <= (vm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte), \
                 tx_code_db_limit_exceeded, \
                 "Database limit exceeded for account=${name}",("name", name(vm_xmax::get().current_message_context->code.code()))); \
      return 1; \
   } \
   DEFINE_INTRINSIC_FUNCTION3(env,remove_##OBJTYPE,remove_##OBJTYPE,i32,u128,scope,u128,table,i32,valueptr) { \
      VERIFY_TABLE(OBJTYPE) \
      UPDATE_RECORD(remove_record, INDEX, sizeof(typename INDEX::value_type::key_type)*INDEX::value_type::number_of_keys); \
      /* ret is -1 if record created, or else it is the length of the data portion of the originally stored */ \
      /* structure (it does not include the key portion */ \
      if (ret == -1) return 0; \
      int64_t& storage = vm_xmax::get().table_storage; \
      storage -= round_to_byte_boundary(KEY_SIZE + ret) + vm_xmax::get().row_overhead_db_limit_bytes; \
      \
      XMAX_ASSERT(storage <= (vm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte), \
                 tx_code_db_limit_exceeded, \
                 "Database limit exceeded for account=${name}",("name", name(vm_xmax::get().current_message_context->code.code()))); \
      return 1; \
   }

#define DEFINE_RECORD_READ_FUNCTION(OBJTYPE, ACTION, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_INTRINSIC_FUNCTION5(env,ACTION##_##FUNCPREFIX##OBJTYPE,ACTION##_##FUNCPREFIX##OBJTYPE,i32,u128,scope,u128,code,u128,table,i32,valueptr,i32,valuelen) { \
      VERIFY_TABLE(OBJTYPE) \
      READ_RECORD(ACTION##_record, INDEX, SCOPE); \
   }

#define DEFINE_RECORD_READ_FUNCTIONS(OBJTYPE, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, load, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, front, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, back, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, next, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, previous, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, lower_bound, FUNCPREFIX, INDEX, SCOPE) \
   DEFINE_RECORD_READ_FUNCTION(OBJTYPE, upper_bound, FUNCPREFIX, INDEX, SCOPE)

	DEFINE_RECORD_UPDATE_FUNCTIONS(i64, key_value_index, 8);
	DEFINE_RECORD_READ_FUNCTIONS(i64, , key_value_index, by_scope_primary);

	DEFINE_RECORD_UPDATE_FUNCTIONS(i128i128, key128x128_value_index, 32);
	DEFINE_RECORD_READ_FUNCTIONS(i128i128, primary_, key128x128_value_index, by_scope_primary);
	DEFINE_RECORD_READ_FUNCTIONS(i128i128, secondary_, key128x128_value_index, by_scope_secondary);

	DEFINE_RECORD_UPDATE_FUNCTIONS(i128i128i128, key128x128x128_value_index, 48);
	DEFINE_RECORD_READ_FUNCTIONS(i128i128i128, primary_, key128x128x128_value_index, by_scope_primary);
	DEFINE_RECORD_READ_FUNCTIONS(i128i128i128, secondary_, key128x128x128_value_index, by_scope_secondary);
	DEFINE_RECORD_READ_FUNCTIONS(i128i128i128, tertiary_, key128x128x128_value_index, by_scope_tertiary);


#define UPDATE_RECORD_STR(FUNCTION) \
  VERIFY_TABLE(str) \
  auto lambda = [&](message_context_xmax* ctx, std::string* keys, char *data, uint32_t datalen) -> int32_t { \
    return ctx->FUNCTION<keystr_value_object>( name(scope), name(ctx->code.code()), table_name, keys, data, datalen); \
  }; \
  const int32_t ret = validate_str<decltype(lambda)>(keyptr, keylen, valueptr, valuelen, lambda);

#define READ_RECORD_STR(FUNCTION) \
  VERIFY_TABLE(str) \
  auto lambda = [&](message_context_xmax* ctx, std::string* keys, char *data, uint32_t datalen) -> int32_t { \
    auto res = ctx->FUNCTION<keystr_value_index, by_scope_primary>( name(scope), name(code), table_name, keys, data, datalen); \
    return res; \
  }; \
  return validate_str<decltype(lambda)>(keyptr, keylen, valueptr, valuelen, lambda);

	DEFINE_INTRINSIC_FUNCTION6(env, store_str, store_str, i32, u128, scope, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
		UPDATE_RECORD_STR(store_record)
			const bool created = (ret == -1);
		auto& storage = vm_xmax::get().table_storage;
		if (created)
		{
			storage += round_to_byte_boundary(keylen + valuelen) + vm_xmax::get().row_overhead_db_limit_bytes;
		}
		else
			// need to calculate the difference between the original rounded byte size and the new rounded byte size
			storage += round_to_byte_boundary(keylen + valuelen) - round_to_byte_boundary(keylen + ret);

		XMAX_ASSERT(storage <= (vm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte),
			tx_code_db_limit_exceeded,
			"Database limit exceeded for account=${name}", ("name", name(vm_xmax::get().current_message_context->code.code())));

		return created ? 1 : 0;
	}
	DEFINE_INTRINSIC_FUNCTION6(env, update_str, update_str, i32, u128, scope, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
		UPDATE_RECORD_STR(update_record)
			if (ret == -1) return 0;
		auto& storage = vm_xmax::get().table_storage;
		// need to calculate the difference between the original rounded byte size and the new rounded byte size
		storage += round_to_byte_boundary(keylen + valuelen) - round_to_byte_boundary(keylen + ret);

		XMAX_ASSERT(storage <= (vm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte),
			tx_code_db_limit_exceeded,
			"Database limit exceeded for account=${name}", ("name", name(vm_xmax::get().current_message_context->code.code())));
		return 1;
	}
	DEFINE_INTRINSIC_FUNCTION4(env, remove_str, remove_str, i32, u128, scope, u128, table, i32, keyptr, i32, keylen) {
		int32_t valueptr = 0, valuelen = 0;
		UPDATE_RECORD_STR(remove_record)
			if (ret == -1) return 0;
		auto& storage = vm_xmax::get().table_storage;
		storage -= round_to_byte_boundary(keylen + ret) + vm_xmax::get().row_overhead_db_limit_bytes;

		XMAX_ASSERT(storage <= (vm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte),
			tx_code_db_limit_exceeded,
			"Database limit exceeded for account=${name}", ("name", name(vm_xmax::get().current_message_context->code.code())));
		return 1;
	}

	DEFINE_INTRINSIC_FUNCTION7(env, load_str, load_str, i32, u128, scope, u128, code, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
		READ_RECORD_STR(load_record)
	}
	DEFINE_INTRINSIC_FUNCTION5(env, front_str, front_str, i32, u128, scope, u128, code, u128, table, i32, valueptr, i32, valuelen) {
		int32_t keyptr = 0, keylen = 0;
		READ_RECORD_STR(front_record)
	}
	DEFINE_INTRINSIC_FUNCTION5(env, back_str, back_str, i32, u128, scope, u128, code, u128, table, i32, valueptr, i32, valuelen) {
		int32_t keyptr = 0, keylen = 0;
		READ_RECORD_STR(back_record)
	}
	DEFINE_INTRINSIC_FUNCTION7(env, next_str, next_str, i32, u128, scope, u128, code, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
		READ_RECORD_STR(next_record)
	}
	DEFINE_INTRINSIC_FUNCTION7(env, previous_str, previous_str, i32, u128, scope, u128, code, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
		READ_RECORD_STR(previous_record)
	}
	DEFINE_INTRINSIC_FUNCTION7(env, lower_bound_str, lower_bound_str, i32, u128, scope, u128, code, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
		READ_RECORD_STR(lower_bound_record)
	}
	DEFINE_INTRINSIC_FUNCTION7(env, upper_bound_str, upper_bound_str, i32, u128, scope, u128, code, u128, table, i32, keyptr, i32, keylen, i32, valueptr, i32, valuelen) {
	
		READ_RECORD_STR(upper_bound_record)
	}

}}