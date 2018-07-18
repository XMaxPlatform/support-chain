#include <libplatform/libplatform.h>
#include <v8.h>

namespace Xmaxplatform {
	namespace Chain {
		/*class V8Msg
		{
		public:
			V8Msg(uint64_t)
			{

			}
			V8Msg()
			{

			}
			
		};

		V8Msg* NewV8MsgFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

		void V8MsgWeakExternalReferenceCallback(const v8::WeakCallbackInfo<V8Msg>& data);

		void V8MsgFunctionInvocationCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

		void SetupV8MsgObjectToJs(v8::Isolate* isolate, v8::Handle<v8::ObjectTemplate> global);*/
		

		void GetMsgData(const v8::FunctionCallbackInfo<v8::Value>& args);

	}
}