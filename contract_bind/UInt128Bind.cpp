#include "Uint128Bind.h"


using namespace v8;

namespace Xmaxplatform {
	namespace Chain {


		//--------------------------------------------------
		V8u128* V8u128::NewV8CppObj(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			return new V8u128();
		}


		//--------------------------------------------------
		void V8u128::ConstructV8Object(const v8::FunctionCallbackInfo<v8::Value>& args)
		{
			if (!args.IsConstructCall())
				return args.GetReturnValue().Set(Undefined(args.GetIsolate()));

			V8u128* cpp_object = NewV8CppObj(args);
			if (!cpp_object)
				return;

			Handle<Object> object = args.This();
			Local<External>  ee = External::New(args.GetIsolate(), cpp_object);
			object->SetInternalField(0, ee);
			Persistent<External> ret = Persistent<External>(args.GetIsolate(), ee);

			ret.SetWeak(cpp_object, WeakExternalReferenceCallback, WeakCallbackType::kParameter);

			args.GetReturnValue().Set(object);
		}


		//--------------------------------------------------
		void V8u128::WeakExternalReferenceCallback(const WeakCallbackInfo<V8u128>& data)
		{
			if (V8u128* cpp_object = data.GetParameter())
			{
				delete cpp_object;
			}
		}

		//--------------------------------------------------
		void V8u128::RegisterWithV8(v8::Isolate* isolate, v8::Handle<ObjectTemplate> global)
		{
			Handle<FunctionTemplate> func_template = FunctionTemplate::New(isolate, &ConstructV8Object);

			func_template->SetClassName(String::NewFromUtf8(
				isolate,
				"V8u128",
				NewStringType::kNormal).ToLocalChecked());
			func_template->InstanceTemplate()->SetInternalFieldCount(1);
			global->Set(String::NewFromUtf8(
				isolate,
				"V8u128",
				NewStringType::kNormal).ToLocalChecked(), func_template);


			Handle<ObjectTemplate> proto_template = func_template->PrototypeTemplate();

			// Here to register methods...

			
		}

}
}