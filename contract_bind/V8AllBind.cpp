#include "V8AllBind.h"
#include "MessageBind.h"
#include "Int64Bind.h"
using namespace v8;
namespace Xmaxplatform {
	namespace Chain {

		void V8AllBind::Setup(v8::Isolate* pIsolate, const v8::Local<v8::ObjectTemplate>& fooGlobal)
		{
			SetupV8MsgObjectToJs(pIsolate, fooGlobal);
			SetupV8i64ObjectToJs(pIsolate, fooGlobal);
		}

		Xmaxplatform::Chain::JsFooBindMap V8AllBind::GetBindFoos(v8::Isolate* pIsolate)
		{
#define bindfoo(fooname)\
	ret.insert(std::pair<std::string, v8::Local<v8::FunctionTemplate>>(#fooname, v8::FunctionTemplate::New(pIsolate, fooname)))

			JsFooBindMap ret;
			return std::move(ret);
		}

	}
}