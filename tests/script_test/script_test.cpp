#ifdef USE_V8
#include "jsvm_xmax.hpp"
#include <V8AllBind.h>

using namespace Xmaxplatform::Chain;
using namespace v8;
int main(int argc, char** argv)
{
	jsvm_xmax::get().V8EnvInit();
	{
		V8AllBind allbind;
		jsvm_xmax::get().V8SetBind(&allbind);

		Isolate::Scope isolate_scope(jsvm_xmax::get().V8GetIsolate());
		// Create a stack-allocated handle scope.
		HandleScope handle_scope(jsvm_xmax::get().V8GetIsolate());
		Local<ObjectTemplate> global = ObjectTemplate::New(jsvm_xmax::get().V8GetIsolate());

		jsvm_xmax::get().V8SetupGlobalObjTemplate(&global);
		std::vector<char> dummyabi;
		{
			const char* code1 = "var i = 33;function init(code,type){while(i>0)i--;return i;} ";
			jsvm_xmax::get().LoadScriptTest(name("test"), code1, dummyabi, fc::sha256("AA"), true);
			jsvm_xmax::get().SetInstructionLimit(50);
			//jsvm_xmax::get().vm_onInit();
			if(jsvm_xmax::get().GetExecutedInsCount()>50)
			{
			
			}
			
		}

		{
			jsvm_xmax::get().SetInstructionLimit(200);
			const char* code2 = "var i = 3;function init(code,type){while(i>0)i--;return i;} ";
			jsvm_xmax::get().LoadScriptTest(name("test"), code2, dummyabi, fc::sha256("BB"), true);
			//jsvm_xmax::get().vm_onInit();
		}
		jsvm_xmax::get().V8ExitContext();
	}
	jsvm_xmax::get().V8EnvDiscard();
}

#else
int main(int argc, char** argv) {
	return 0;
}

#endif