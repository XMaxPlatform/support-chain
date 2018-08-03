#ifdef USE_V8
#include "jsvm_util.hpp"
#include <iostream>
#include <blockchain_exceptions.hpp>
#include "jsvm_objbind/V8BindBase.h"

using namespace v8;
namespace Xmaxplatform {

	namespace Chain {

		PersistentCpyableContext CreateJsContext(v8::Isolate* pIsolate, v8::Local<v8::ObjectTemplate>& global)
		{
			V8_ParseWithOutPlugin();
			Local<Context> context = Context::New(pIsolate, NULL, global);
			PersistentCpyableContext ret(pIsolate, context);
			return ret;
		}

		void EnterJsContext(v8::Isolate* pIsolate, v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>>& context)
		{
			Local<Context> localContext = context.Get(pIsolate);
			localContext->Enter();
		}

		void ExitJsContext(v8::Isolate* pIsolate, v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>>& context)
		{
			Local<Context> localContext = context.Get(pIsolate);
			localContext->Exit();
		}

		void BindJsFoos(Isolate* pIsolate, const Local<ObjectTemplate>& fooGlobal, const std::map<std::string, Local<FunctionTemplate>>& foosToBind)
		{
			for (const std::pair<std::string, Local<FunctionTemplate> >& foobind : foosToBind)
			{
				fooGlobal->Set(
					v8::String::NewFromUtf8(pIsolate, foobind.first.c_str(), v8::NewStringType::kNormal).ToLocalChecked(),
					foobind.second);
			}
		}

		Local<Script> CompileJsCode(Isolate* pIsolate,const Local<Context>& context, char* jsCode)
		{
			Local<String> source =
				String::NewFromUtf8(pIsolate, jsCode,
					NewStringType::kNormal).ToLocalChecked();
			V8_ParseWithPlugin();
			MaybeLocal<Script> temp = Script::Compile(context, source);
			V8_ParseWithOutPlugin();
			if (temp.IsEmpty())
			{
				std::cerr << "js compile failed" << std::endl;
			}
			Local<Script> script = temp.ToLocalChecked();
			if (script.IsEmpty()) {
				std::cerr << "js compile failed" << std::endl;
			}
			TryCatch trycatch(pIsolate);
			Local<Value> v = script->Run();
			if (v.IsEmpty()) {
				Local<Value> exception = trycatch.Exception();
				String::Utf8Value exception_str(exception);
				printf("Exception: %s\n", *exception_str);
				// ...
			}
			return script;
		}

		v8::Handle<v8::Value> CallJsFoo(Isolate* pIsolate, const Local<Context>& context,const char* fooname, unsigned int argc,Handle<v8::Value>* params)
		{
			Handle<String> js_func_name = String::NewFromUtf8(pIsolate, fooname, NewStringType::kNormal).ToLocalChecked();
			Handle<v8::Value>  js_func_val = context->Global()->Get(js_func_name);
			if (!js_func_val->IsFunction())
			{
				std::cerr << "Can't find js funcion init()" << std::endl;
				return  Undefined(pIsolate);
			}
			else
			{
				V8_ParseWithPlugin();
				Handle<Function> js_func = Handle<Function>::Cast(js_func_val);
				TryCatch trycatch(pIsolate);
				Handle<v8::Value> hResult = js_func->Call(context->Global(), argc, params);
				if (hResult.IsEmpty()) {
					Local<Value> exception = trycatch.Exception();
					String::Utf8Value exception_str(exception);
					printf("Exception: %s\n", *exception_str);
					throw script_runout();
				}	
				V8_ParseWithOutPlugin();
				return hResult;
			}
		}

		Handle<v8::Value> I64Cpp2JS(Isolate* isolate, const Local<Context>& context, int64_t v)
		{
			Handle<String> js_data = String::NewFromUtf8(isolate, "V8i64", NewStringType::kNormal).ToLocalChecked();
			Handle<v8::Value> js_data_value = context->Global()->Get(js_data);

			bool bIsObject = js_data_value->IsObject();
			if (bIsObject)
			{
				Handle<Object> js_data_object = Handle<Object>::Cast(js_data_value);

				int* pvarr = (int*)&v;

				Handle<v8::Value>  argcodev[2];
				argcodev[0] = Int32::New(isolate, pvarr[1]);
				argcodev[1] = Int32::New(isolate, pvarr[0]);

				Handle<v8::Value> codeObj = js_data_object->CallAsConstructor(2, argcodev);
				return codeObj;
			}
			return Undefined(isolate);
		}

		int64_t  I64JS2CPP(v8::Isolate* isolate, v8::Handle<v8::Value> obj)
		{
			bool bIsObject = obj->IsObject();
			if (bIsObject)
			{
				Handle<Object> js_data_object = Handle<Object>::Cast(obj);

				Local<External> wrap = Local<External>::Cast(js_data_object->GetInternalField(0));
				void* ptr = wrap->Value();
				int64_t ret = 0;
				int64_t* pv8i64 = static_cast<int64_t*>(ptr);
				ret = (int64_t) *pv8i64;
				
				return ret;
			}
			return 0;
		}

		bool IsType(v8::Handle<v8::Value> obj, Basetypes::name _type)
		{
			bool bIsObject = obj->IsObject();
			if (bIsObject) {
				Handle<Object> jsObj = Handle<Object>::Cast(obj);
				v8::Handle<External> ptr = Local<External>::Cast(jsObj->GetInternalField(0));
				V8BindBase* basePtr = (V8BindBase*)ptr->Value();
				if (basePtr->GetTypeID() == _type)
				{
					return true;
				}
			}
			return false;
		}

		namespace FooBind
		{
			void FooBind::exportFoo(const FunctionCallbackInfo<v8::Value>& args)
			{
				bool first = true;
				//for (int i = 0; i < args.Length(); i++) {
					v8::HandleScope handle_scope(args.GetIsolate());
					HandleScope scope(args.GetIsolate());
					args.GetIsolate()->ThrowException(v8::Exception::Error(String::NewFromUtf8(args.GetIsolate(), "error string here")));

			}
		}



	}
}
#endif