#include <libplatform/libplatform.h>
#include <v8.h>

namespace Xmaxplatform {
	namespace Chain {
		class V8Msg
		{
		public:
			V8Msg(uint64_t)
			{

			}
			V8Msg()
			{

			}

			void* dataPtr;
			int dataLength;
			
		};

		V8Msg* NewV8MsgFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

		void V8MsgWeakExternalReferenceCallback(v8::Persistent<v8::Value>, void* parameter);

		void V8MsgFunctionInvocationCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

		void SetupV8MsgObjectToJs(v8::Isolate* isolate, v8::Handle<v8::ObjectTemplate> global);

		void GetData(const v8::FunctionCallbackInfo<v8::Value>& args);

	}
}