#pragma once
#include <libplatform/libplatform.h>
#include <v8.h>
#include "jsvm_objbind/V8Bind.h"


namespace Xmaxplatform {
	namespace Chain {
		class V8AllBind:public V8Bind
		{
		public:

			V8AllBind()
			{

			}

			virtual ~V8AllBind()
			{

			}

			virtual void Setup(v8::Isolate* pIsolate, const v8::Local<v8::ObjectTemplate>& fooGlobal);
			virtual JsFooBindMap GetBindFoos(v8::Isolate* pIsolate);
			
		};

	}
}