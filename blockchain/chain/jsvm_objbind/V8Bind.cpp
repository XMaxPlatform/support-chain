#include "V8Bind.h"
using namespace v8;
namespace Xmaxplatform {
	namespace Chain {

		void V8Bind::Setup(v8::Isolate* pIsolate, const v8::Local<v8::ObjectTemplate>& fooGlobal)
		{

		}

		JsFooBindMap V8Bind::GetBindFoos(v8::Isolate* pIsolate)
		{
#define bindfoo(fooname)\
	ret.insert(std::pair<std::string, v8::Local<v8::FunctionTemplate>>(#fooname, v8::FunctionTemplate::New(pIsolate, fooname)))

			JsFooBindMap ret;
			return std::move(ret);
		}

	}
}