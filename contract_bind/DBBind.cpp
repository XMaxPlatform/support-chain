#include "DBBind.h"
#include "UInt128Bind.h"
#include "jsvm_util.hpp"
#include "jsvm_xmax.hpp"
using namespace v8;

namespace Xmaxplatform {
	namespace Chain {

		int64_t db_round_to_byte_boundary(int64_t bytes)
		{
			const unsigned int byte_boundary = sizeof(uint64_t);
			int64_t remainder = bytes % byte_boundary;
			if (remainder > 0)
				bytes += byte_boundary - remainder;
			return bytes;
		}

		template <typename Function, typename KeyType, int numberOfKeys>
		int32_t validate(void* keyptr, void* valueptr, int32_t valuelen, Function func) {

			static const uint32_t keylen = numberOfKeys * sizeof(KeyType);

			FC_ASSERT(valuelen >= keylen, "insufficient data passed");

			auto& jsvm = jsvm_xmax::get();
			FC_ASSERT(jsvm.current_message_context, "no apply context found");

// 			char* value = memoryArrayPtr<char>(jsvm.current_memory, valueptr, valuelen);
// 			KeyType*  keys = reinterpret_cast<KeyType*>(value);

			//valuelen -= keylen;
			//value += keylen;
			KeyType* keys = (KeyType*)keyptr;
			char* value = (char*)valueptr;
			return func(jsvm.current_message_context, keys, value, valuelen);
		}


#define VERIFY_TABLE(TYPE) \
   const auto table_name = name(table); \
   auto& jsvm  = jsvm_xmax::get(); \
   if (jsvm.table_key_types) \
   { \
      auto table_key = jsvm.table_key_types->find(table_name); \
      if (table_key == jsvm.table_key_types->end()) \
      { \
         FC_ASSERT(!jsvm.tables_fixed, "abi did not define table ${t}", ("t", table_name)); \
         jsvm.table_key_types->emplace(std::make_pair(table_name,jsvm_xmax::TYPE)); \
      } \
      else \
      { \
         FC_ASSERT(jsvm_xmax::TYPE == table_key->second, "abi definition for ${table} expects \"${type}\", but code is requesting \"" #TYPE "\"", ("table",table_name)("type",jsvm_xmax::to_type_name(table_key->second))); \
      } \
   }

#define READ_RECORD(READFUNC, INDEX, SCOPE,keyptr,valueptr,valuelen) \
   auto lambda = [&](message_context_xmax* ctx, INDEX::value_type::key_type* keys, char *data, uint32_t datalen) -> int32_t { \
      auto res = ctx->READFUNC<INDEX, SCOPE>( name(scope), name(code), table_name, keys, data, datalen); \
      if (res >= 0) res += INDEX::value_type::number_of_keys*sizeof(INDEX::value_type::key_type); \
      return res; \
   }; \
   const int32_t ret = validate<decltype(lambda), INDEX::value_type::key_type, INDEX::value_type::number_of_keys>(keyptr,valueptr, valuelen, lambda);

#define UPDATE_RECORD(UPDATEFUNC, INDEX,keyptr,valueptr, DATASIZE) \
   auto lambda = [&](message_context_xmax* ctx, INDEX::value_type::key_type* keys, char *data, uint32_t datalen) -> int32_t { \
      return ctx->UPDATEFUNC<INDEX::value_type>( name(scope), name(ctx->code.code()), table_name, keys, data, datalen); \
   }; \
   const int32_t ret = validate<decltype(lambda), INDEX::value_type::key_type, INDEX::value_type::number_of_keys>(keyptr,valueptr, DATASIZE, lambda);


		void LoadRecord(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 5)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "argument count error!")));
			}
			name scope , code, table;

			HandleScope handlescope(args.GetIsolate());
			{
				Handle<v8::Value> js_data_value = args[0];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					scope = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
				}
			}

			{
				Handle<v8::Value> js_data_value = args[1];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					code = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
				}
			}

			{
				Handle<v8::Value> js_data_value = args[2];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					table = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
				}
			}

			int64_t i64key;
			{
				Handle<v8::Value> js_data_value = args[2];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					i64key = I64JS2CPP(args.GetIsolate(), js_data_value);
				}
			}
			int64_t value;
			VERIFY_TABLE(i64)
			READ_RECORD(load_record, key_value_index, by_scope_primary, &i64key, &value, 8);

			args.GetReturnValue().Set(I64Cpp2JS(args.GetIsolate(),args.GetIsolate()->GetCurrentContext(),value));
		}

		void StoreRecord(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 4)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "argument count error!")));
			}
			name scope, table;

			HandleScope handlescope(args.GetIsolate());
			{
				Handle<v8::Value> js_data_value = args[0];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					scope = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
				}
			}

			{
				Handle<v8::Value> js_data_value = args[1];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					table = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
				}
			}

			int64_t i64key;
			{
				Handle<v8::Value> js_data_value = args[2];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					i64key = I64JS2CPP(args.GetIsolate(), js_data_value);
				}
			}

			int64_t value;
			{
				Handle<v8::Value> js_data_value = args[3];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					value = I64JS2CPP(args.GetIsolate(), js_data_value);
				}
			}
			
			VERIFY_TABLE(i64)
			UPDATE_RECORD(store_record, key_value_index, &i64key,&value, 8);
			const bool created = (ret == -1); 
			int64_t& storage = jsvm_xmax::get().table_storage; 
			if (created) 
				storage += db_round_to_byte_boundary(8) + jsvm_xmax::get().row_overhead_db_limit_bytes; 
			else 
				storage += db_round_to_byte_boundary(8) - db_round_to_byte_boundary(8 + ret);
					
			//XMAX_ASSERT(storage <= (jsvm_xmax::get().per_code_account_max_db_limit_mbytes * bytes_per_mbyte), tx_code_db_limit_exceeded, 
			//			"Database limit exceeded for account=${name}", ("name", name(jsvm_xmax::get().current_message_context->code.code())));

		}

		void UpdateRecord(const v8::FunctionCallbackInfo<v8::Value>& args)
		{	
		}

		void RemoveRecord(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
		}
	}
}