#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>
using namespace v8;

namespace Xmaxplatform {
	namespace Chain {


		void LoadRecord(const v8::FunctionCallbackInfo<v8::Value>& args);
		void StoreRecord(const v8::FunctionCallbackInfo<v8::Value>& args);

	}
}