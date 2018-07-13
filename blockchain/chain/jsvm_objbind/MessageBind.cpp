#include "MessageBind.h"
using namespace v8;
namespace Xmaxplatform {
	namespace Chain {

		V8Msg* NewV8MsgFunction(const FunctionCallbackInfo<v8::Value>& args)
		{
			return nullptr;
		}

		void V8MsgWeakExternalReferenceCallback(Persistent<v8::Value>, void* parameter)
		{

		}

		void V8MsgFunctionInvocationCallback(const FunctionCallbackInfo<v8::Value>& args)
		{

		}

		void SetupV8MsgObjectToJs(Isolate* isolate, Handle<ObjectTemplate> global)
		{

		}

		void GetData(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			Local<Object> self = args.Holder();

			Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
			void* ptr = wrap->Value();

//			args.GetReturnValue().Set(Int32::New(args.GetIsolate(), static_cast<V8Msg*>(ptr)->data[0]));
		}

	}
}