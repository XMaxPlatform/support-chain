#include "MessageBind.h"
#include "jsvm_util.hpp"
#include "jsvm_xmax.hpp"
#include <../../blockchain_plugin/include/blockchain_plugin.hpp>
#include <../../../blockchain/chain/include/blockchain_types.hpp>
#include <fc/variant.hpp>
#include <chain_xmax.hpp>
#include "jsvm_objbind/UInt128Bind.h"

using namespace v8;
namespace Xmaxplatform {
	namespace Chain {
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
			if (args.Length()!=4)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "argument error!")));
			}
			name code, type;
			const char* key;
			const char* totype;
			
			{
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

				v8::String::Utf8Value str0(args[2]);
				key = StringJS2CPP(str0);
				
				v8::String::Utf8Value str1(args[3]);
				totype = StringJS2CPP(str1);
				

				if (strcmp(totype,"int")==0)
				{
					int ret;
					MsgGet(code, type, jsvm_xmax::get().current_validate_context->msg.data, key, ret);
					args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), ret));
					return;
				}
				else if (strcmp(totype, "i64") == 0)
				{
					int64_t ret;
					MsgGet(code, type, jsvm_xmax::get().current_validate_context->msg.data, key, ret);
					args.GetReturnValue().Set(I64Cpp2JS(args.GetIsolate(), args.GetIsolate()->GetCurrentContext(), ret));
					return;
				}
				else if (strcmp(totype, "u128") == 0) {
					uint128 ret;					
					MsgGet(code, type, jsvm_xmax::get().current_validate_context->msg.data, key, ret);
					args.GetReturnValue().Set(CppObjToJs(args.GetIsolate(), args.GetIsolate()->GetCurrentContext(), (V8u128)ret));
					return;
				}

				args.GetReturnValue().SetUndefined();
			}
		}
	}
}