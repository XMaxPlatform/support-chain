#pragma once
#include <libplatform/libplatform.h>
#include <v8.h>

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

			virtual void Setup(v8::Isolate* pIsolate, const v8::Local<v8::ObjectTemplate>& fooGlobal)
			{

			}

			
		};

	}
}