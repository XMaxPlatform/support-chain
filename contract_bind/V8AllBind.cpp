#include <map>
#include "V8AllBind.h"
#include "MessageBind.h"
#include "Int64Bind.h"
#include "jsvm_objbind/UInt128Bind.h"
#include "DBBind.h"
#include "UtilBind.h"

using namespace v8;
namespace Xmaxplatform {
	namespace Chain {

		void V8AllBind::Setup(v8::Isolate* pIsolate, const v8::Local<v8::ObjectTemplate>& fooGlobal)
		{
			//SetupV8MsgObjectToJs(pIsolate, fooGlobal);
			SetupV8i64ObjectToJs(pIsolate, fooGlobal);
			V8u128::RegisterWithV8(pIsolate, fooGlobal);
			V8TableI128::RegisterWithV8(pIsolate, fooGlobal);
		}

		JsFooBindMap V8AllBind::GetBindFoos(v8::Isolate* pIsolate)
		{
#define bindfoo(fooname)\
	ret.insert(std::pair<std::string, v8::Local<v8::FunctionTemplate>>(#fooname, v8::FunctionTemplate::New(pIsolate, fooname)))

			JsFooBindMap ret;
			bindfoo(GetMsgData);
			bindfoo(LoadRecord);
			bindfoo(StoreRecord);
			bindfoo(StrToName);
			bindfoo(StrIsName);
			
			return std::move(ret);
		}

	}
}