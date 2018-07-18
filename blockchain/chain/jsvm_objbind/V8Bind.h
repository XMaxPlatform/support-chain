#pragma once
#include <libplatform/libplatform.h>
#include <v8.h>
#include "jsvm_util.hpp"

namespace Xmaxplatform {
	namespace Chain {
		class V8Bind
		{
		public:

			V8Bind()
			{

			}

			virtual ~V8Bind()
			{

			}

			virtual void Setup(v8::Isolate* pIsolate, const v8::Local<v8::ObjectTemplate>& fooGlobal);
			virtual JsFooBindMap GetBindFoos(v8::Isolate* pIsolate);
			
		};

	}
}