#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>
using namespace v8;

namespace Xmaxplatform {
	namespace Chain {

		 
		void StrToName(const v8::FunctionCallbackInfo<v8::Value>& args);

		void StrIsName(const v8::FunctionCallbackInfo<v8::Value>& args);

	}
}