#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>

namespace Xmaxplatform {

	namespace Chain {

		using PersistentCpyableContext = v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> ;
		
		typedef std::function<void( const v8::Local<v8::ObjectTemplate>&, const v8::Local<v8::Context>&, const v8::Context::Scope&)> DoWorkInJsCtx;

		typedef std::map<std::string, v8::Local<v8::FunctionTemplate>> JsFooBindMap;
		
		void EnterJsContext(v8::Isolate* pIsolate,v8::Local<v8::ObjectTemplate>& global,DoWorkInJsCtx dowork);

		PersistentCpyableContext CreateJsContext(v8::Isolate* pIsolate, v8::Local<v8::ObjectTemplate>& global);

		void EnterJsContext(v8::Isolate* pIsolate, v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>>& context);

		void ExitJsContext(v8::Isolate* pIsolate, v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>>& context);

		void BindJsFoos(v8::Isolate* pIsolate,const v8::Local<v8::ObjectTemplate>& fooGlobal, const JsFooBindMap& foosToBind);

		v8::Local<v8::Script> CompileJsCode(v8::Isolate* pIsolate, const v8::Local<v8::Context>& context,char* jsCode);

		v8::Handle<v8::Value> CallJsFoo(v8::Isolate* pIsolate, const v8::Local<v8::Context>& context, const char* fooname, unsigned int argc, v8::Handle<v8::Value>* params);

		v8::Handle<v8::Value> I64Cpp2JS(v8::Isolate* isolate, const v8::Local<v8::Context>& context, int64_t v);
		int64_t I64JS2CPP(v8::Isolate* isolate, v8::Handle<v8::Value> obj);

		template <typename ObjType>
		inline ObjType* JsObjToCpp(v8::Isolate* isolate, v8::Handle<v8::Value> obj) {
			bool bIsObject = obj->IsObject();
			if (bIsObject) {			
				return ObjType::Unwrap(Handle<Object>::Cast(obj));				
			}
			else {
				return nullptr;
			}		
		}

		template <typename ObjType>
		v8::Handle<v8::Value> CppObjToJs(v8::Isolate* isolate, const v8::Local<v8::Context>& context, ObjType* cpp_object)
		{
			Handle<String> js_data = String::NewFromUtf8(isolate, ObjType::TypeName(), NewStringType::kNormal).ToLocalChecked();
			Handle<v8::Value> js_data_value = context->Global()->Get(js_data);

			bool bIsObject = js_data_value->IsObject();
			if (bIsObject)
			{
				
				Handle<Object> js_data_object = Handle<Object>::Cast(js_data_value);
				ObjType::Wrap(isolate, cpp_object, js_data_object);
				return js_data_object;								
			}

			return Undefined(isolate);
		}


		inline const char* StringJS2CPP(const v8::String::Utf8Value& value) {
			return *value ? *value : "<string conversion failed>";
		}

		namespace FooBind
		{
			void exportFoo(const v8::FunctionCallbackInfo<v8::Value>& args);

			inline JsFooBindMap GetBindFoos(v8::Isolate* pIsolate)
			{
#define bindfoo(fooname)\
	ret.insert(std::pair<std::string, v8::Local<v8::FunctionTemplate>>(#fooname, v8::FunctionTemplate::New(pIsolate, fooname)))

				JsFooBindMap ret;
				bindfoo(exportFoo);
				return std::move(ret);
			}
		}

	}
}