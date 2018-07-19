#include "MessageBind.h"
#include "jsvm_util.hpp"
#include "jsvm_xmax.hpp"
#include <../../blockchain_plugin/include/blockchain_plugin.hpp>
#include <../../../blockchain/chain/include/blockchain_types.hpp>
#include <fc/variant.hpp>
#include <chain_xmax.hpp>
#include "UInt128Bind.h"

using namespace v8;
namespace Xmaxplatform {
	namespace Chain {
		/*
		V8Msg* NewV8MsgFunction(const FunctionCallbackInfo<v8::Value>& args)
		{
			return new V8Msg();
		}

		void V8MsgWeakExternalReferenceCallback(const WeakCallbackInfo<V8Msg>& data)
		{
			if (V8Msg* cpp_object = data.GetParameter())
			{
				delete cpp_object;
			}
		}

		void V8MsgFunctionInvocationCallback(const FunctionCallbackInfo<v8::Value>& args)
		{
			if (!args.IsConstructCall())
				return args.GetReturnValue().Set(Undefined(args.GetIsolate()));

			V8Msg* cpp_object = NewV8MsgFunction(args);
			if (!cpp_object)
				return;

			Handle<Object> object = args.This();
			Local<External>  ee = External::New(args.GetIsolate(), cpp_object);
			object->SetInternalField(0, ee);
			Persistent<External> ret = Persistent<External>(args.GetIsolate(), ee);

			ret.SetWeak(cpp_object, V8MsgWeakExternalReferenceCallback, WeakCallbackType::kParameter);

			args.GetReturnValue().Set(Undefined(args.GetIsolate()));
		}

		void SetupV8MsgObjectToJs(Isolate* isolate, Handle<ObjectTemplate> global)
		{
			Handle<FunctionTemplate> msg_templ = FunctionTemplate::New(isolate, &V8MsgFunctionInvocationCallback);

			msg_templ->SetClassName(String::NewFromUtf8(
				isolate,
				"V8msg",
				NewStringType::kNormal).ToLocalChecked());
			msg_templ->InstanceTemplate()->SetInternalFieldCount(1);

			Handle<ObjectTemplate> msg_proto = msg_templ->PrototypeTemplate();

			msg_proto->Set(String::NewFromUtf8(
				isolate,
				"GetData",
				NewStringType::kNormal).ToLocalChecked(), FunctionTemplate::New(isolate, GetData));

			msg_proto->Set(String::NewFromUtf8(
				isolate,
				"V8i64",
				NewStringType::kNormal).ToLocalChecked(), msg_templ);
		}*/

		template<typename T>
		void MsgGet(name code, name type, const vector<char>& bin, const char* key, T& result)
		{
			Chain::chain_xmax& cc = app().get_plugin<blockchain_plugin>().getchain();
			fc::variant var = cc.message_from_binary(code, type, bin);

			const variant_object& vo = var.get_object();
			if (vo.contains(key))
			{
				fc::from_variant(vo[key], result);
			}
		}

		void GetMsgData(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			Local<Object> self = args.Holder();

			Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
			void* ptr = wrap->Value();

			if (args.Length()!=4)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "argument error!")));
			}
			name code, type;
			const char* key;
			const char* totype;
			
			{
				v8::HandleScope handle_scope(args.GetIsolate());
				HandleScope scope(args.GetIsolate());
				{
					Handle<v8::Value> js_data_value = args[0];

					bool bIsObject = js_data_value->IsObject();
					if (bIsObject)
					{						
						code = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
					}
				}
				
				{
					Handle<v8::Value> js_data_value = args[1];

					bool bIsObject = js_data_value->IsObject();
					if (bIsObject)
					{
						type = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
					}
				}

				{
					v8::String::Utf8Value str(args[2]);
					key = StringJS2CPP(str);
				}
				
				{
					v8::String::Utf8Value str(args[3]);
					totype = StringJS2CPP(str);
				}
				if (strcmp(totype,"int")==0)
				{
					int ret;
					MsgGet(code, type, jsvm_xmax::get().current_validate_context->msg.data, key, ret);
					args.GetReturnValue().Set(Int32::New(args.GetIsolate(), ret));
					return;
				}
				
				if (strcmp(totype, "i64") == 0)
				{
					int64_t ret;
					MsgGet(code, type, jsvm_xmax::get().current_validate_context->msg.data, key, ret);
					args.GetReturnValue().Set(I64Cpp2JS(args.GetIsolate(), args.GetIsolate()->GetCurrentContext(), ret));
					return;
				}

				if (strcmp(totype, "u128") == 0) {
					uint128 ret;					
					MsgGet(code, type, jsvm_xmax::get().current_validate_context->msg.data, key, ret);
					V8u128* pV8u128 = new V8u128(ret);
					args.GetReturnValue().Set(CppObjToJs(args.GetIsolate(), args.GetIsolate()->GetCurrentContext(), pV8u128));
					return;
				}

				args.GetReturnValue().SetUndefined();
			}
		}
	}
}