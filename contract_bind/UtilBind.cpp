#include "UtilBind.h"
#include "basetypes.hpp"
#include "jsvm_util.hpp"
#include "jsvm_objbind/UInt128Bind.h"
namespace Xmaxplatform {
	namespace Chain {

		void StrToName(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 1)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "StrToName:argument count error!")));
			}

			HandleScope scope(args.GetIsolate());
			
			const char* ustr128;
			{
				v8::String::Utf8Value str(args[0]);
				ustr128 = StringJS2CPP(str);

				Basetypes::name ret (ustr128);

				v8::Handle<v8::Value> jsret = CppObjToJs<V8u128>(args.GetIsolate(), args.GetIsolate()->GetCurrentContext(), (V8u128)ret);

				args.GetReturnValue().Set(jsret);
			}
		}
		void StrIsName(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (args.Length() != 2)
			{
				args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "StrToName:argument count error!")));
			}

			HandleScope scope(args.GetIsolate());
			Basetypes::name lvalue, rvalue;
			const char* ustr128;
			{
				v8::String::Utf8Value str(args[0]);
				ustr128 = StringJS2CPP(str);

				lvalue = Basetypes::name (ustr128);

			}

			{
				Handle<v8::Value> js_data_value = args[1];

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					rvalue = *JsObjToCpp<V8u128>(args.GetIsolate(), js_data_value);
				}

			}
			if (lvalue == rvalue)
			{
				args.GetReturnValue().Set(true);
			}
			else
			{
				args.GetReturnValue().Set(false);
			}
		}
		

	}
}