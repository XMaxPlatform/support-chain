#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>


namespace Xmaxplatform {
	namespace Chain {
		
		template <typename ObjType>
		class V8BindObject {
		public:
			//V8 bind
			static ObjType* NewV8CppObj(const v8::FunctionCallbackInfo<v8::Value>& args) { static_assert(false, "Need to implemented in the derived class."); }
			static void RegisterWithV8(v8::Isolate* isolate, v8::Handle<v8::ObjectTemplate> global) { static_assert(false, "Need to implemented in the derived class."); }
			static void ConstructV8Object(const v8::FunctionCallbackInfo<v8::Value>& args) { static_assert(false, "Need to implemented in the derived class."); }
			static void WeakExternalReferenceCallback(const v8::WeakCallbackInfo<ObjType>& data) { static_assert(false, "Need to implemented in the derived class."); }
		};
	} // namespace Chain
}