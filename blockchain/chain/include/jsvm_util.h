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