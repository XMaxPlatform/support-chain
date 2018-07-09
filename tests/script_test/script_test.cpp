#ifdef USE_V8
#include "jsvm_xmax.h"


int main(int argc, char** argv)
{
	Xmaxplatform::Chain::jsvm_xmax::get().V8EnvInit();
	{
		v8::Isolate::Scope isolate_scope(Xmaxplatform::Chain::jsvm_xmax::get().V8GetIsolate());
		// Create a stack-allocated handle scope.
		v8::HandleScope handle_scope(Xmaxplatform::Chain::jsvm_xmax::get().V8GetIsolate());
		v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(Xmaxplatform::Chain::jsvm_xmax::get().V8GetIsolate());
		Xmaxplatform::Chain::jsvm_xmax::get().V8SetupGlobalObjTemplate(&global);
	}
	Xmaxplatform::Chain::jsvm_xmax::get().V8EnvDiscard();
}


#endif