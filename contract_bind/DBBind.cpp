#include "DBBind.h"
#include "jsvm_objbind/UInt128Bind.h"
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
			uint32_t testiii = keylen;
			int numkeiii = numberOfKeys;

			auto& jsvm = jsvm_xmax::get();
			FC_ASSERT(jsvm.current_message_context, "no apply context found");

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
			VERIFY_TABLE(i128)
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
					V8u128* ret = JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
					scope = *ret;
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
			uint128  test = 0;
			name ikey =(uint128) 0;
			{
				Handle<v8::Value> js_data_value = args[2];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					if (IsType(js_data_value,V8u128::TypeID()) )
					{
						ikey = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
					}
					
				}
				else
				{
					Local<v8::Integer> v8int = Local<v8::Integer>::Cast(js_data_value);
					ikey = (uint128)v8int->Value();
				}
			}

			int64_t value;
			{
				Local<v8::Value> js_data_value = args[3];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					value = I64JS2CPP(args.GetIsolate(), js_data_value);
				}
				else
				{
					Local<v8::Integer> v8int = Local<v8::Integer>::Cast(js_data_value);
					value = v8int->Value();;
				}
			}
			
			VERIFY_TABLE(i128)
			UPDATE_RECORD(store_record, key_value_index, &ikey,&value, 8);
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


		//--------------------------------------------------
		V8TableI128* V8TableI128::NewV8CppObj(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 3)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "V8TableI128::Ctr() argument count error!")));
			}

			name scope, code, table;

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

			return new V8TableI128(scope,code,table);
		}


		//--------------------------------------------------
		void V8TableI128::ConstructV8Object(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (!args.IsConstructCall())
				return args.GetReturnValue().Set(Undefined(args.GetIsolate()));

			V8TableI128* cpp_object = nullptr;
			if (args.Length() == 1)
			{
				Local<Object> self = args.Holder();
				Local<External> wrap = Local<External>::Cast(args[0]);
				cpp_object = (V8TableI128*)wrap->Value();
			}
			else 
			{
				NewV8CppObj(args);
			}

			if (!cpp_object)
				return;

			Handle<Object> object = args.This();
			Wrap(args.GetIsolate(), cpp_object, object);
			args.GetReturnValue().Set(object);
		}
		//--------------------------------------------------
		void V8TableI128::WeakExternalReferenceCallback(const WeakCallbackInfo<V8TableI128>& data)
		{
			if (V8TableI128* cpp_object = data.GetParameter())
			{
				delete cpp_object;
			}
		}

		//--------------------------------------------------
		void V8TableI128::RegisterWithV8(v8::Isolate* isolate, v8::Handle<ObjectTemplate> global)
		{
			Handle<FunctionTemplate> func_template = FunctionTemplate::New(isolate, &ConstructV8Object);

			func_template->SetClassName(String::NewFromUtf8(
				isolate,
				TypeName(),
				NewStringType::kNormal).ToLocalChecked());
			func_template->InstanceTemplate()->SetInternalFieldCount(1);
			global->Set(String::NewFromUtf8(
				isolate,
				TypeName(),
				NewStringType::kNormal).ToLocalChecked(), func_template);


			Handle<ObjectTemplate> proto_template = func_template->PrototypeTemplate();

			// Here to register methods...
		}

		void V8TableI128::Store(name key, void* data, uint32_t dataLen)
		{
			name scope, table;
			scope = mScope;
			table = mTable;

			VERIFY_TABLE(i128)
			UPDATE_RECORD(store_record, key_value_index, &key, data, dataLen);
			const bool created = (ret == -1);
			int64_t& storage = jsvm_xmax::get().table_storage;
			if (created)
				storage += db_round_to_byte_boundary(8) + jsvm_xmax::get().row_overhead_db_limit_bytes;
			else
				storage += db_round_to_byte_boundary(8) - db_round_to_byte_boundary(8 + ret);
		}

		void V8TableI128::Load(name key, void* data, uint32_t dataLen)
		{
			name scope,code , table;
			scope = mScope;
			code = mCode;
			table = mTable;

			VERIFY_TABLE(i128)
			READ_RECORD(load_record, key_value_index, by_scope_primary, &key, data, dataLen);
		}

		void V8TableI128::CacheDataType(uint32_t length, Basetypes::name type)
		{
			mDataLength = length;
			mDataType = type;
		}

		void Store(const FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 2)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "V8TableI128::Store() argument count error!")));
			}

			Local<Object> self = args.Holder();

			Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
			V8TableI128* selfPtr = (V8TableI128*)wrap->Value();
			
			HandleScope handlescope(args.GetIsolate());

			name key;
			Handle<v8::Value> js_data_value = args[0];
			bool bIsObject = js_data_value->IsObject();
			if (bIsObject)
			{
				key = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
			}
			
			js_data_value = args[1];
			bIsObject = js_data_value->IsObject();
			uint32_t length;
			void * dataPtr;
			name datatype;
			if (bIsObject)
			{
				if (IsType(js_data_value, V8u128::TypeID()))
				{ 
					auto value = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
					dataPtr = &value;
					length = sizeof(value);
					datatype = V8u128::TypeID();
				}
	
			}
			else
			{
				Local<v8::Int32> v8int = Local<v8::Int32>::Cast(js_data_value);
				auto value = v8int->Value();
				dataPtr = &value;
				length = sizeof(value);
			}
			selfPtr->Store(key, dataPtr, length);
			selfPtr->CacheDataType(length, datatype);
			args.GetReturnValue().Set(Undefined(args.GetIsolate()));
		}

		void Load(const FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 1)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "V8TableI128::Load() argument count error!")));
			}

			Local<Object> self = args.Holder();

			Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
			V8TableI128* selfPtr = (V8TableI128*)wrap->Value();

			HandleScope handlescope(args.GetIsolate());

			name key;
			Handle<v8::Value> js_data_value = args[0];
			bool bIsObject = js_data_value->IsObject();
			if (bIsObject)
			{
				key = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
			}

			js_data_value = args[1];
			bIsObject = js_data_value->IsObject();
			uint32_t datalength = selfPtr->GetCacheDataLength();
			name datatype = selfPtr->GetCacheDataType();
			char * dataPtr = new char[datalength];
			
			selfPtr->Load(key, dataPtr, datalength);

			Local<Value> retValue;
			if (datalength==8)
			{
				retValue = Int32::New(args.GetIsolate(), (int)*dataPtr);
			}
			else if(datatype == V8u128::TypeID())
			{
				retValue = CppObjToJs<V8u128>(args.GetIsolate(), args.GetIsolate()->GetCurrentContext(), (V8u128)*dataPtr);
			}

			args.GetReturnValue().Set(retValue);
		}
	}
}