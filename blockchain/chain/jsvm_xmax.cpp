#ifdef USE_V8

#include "jsvm_xmax.h"
/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <boost/function.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <jsvm_xmax.h>
#include <chain_xmax.hpp>
#include <vm_native_interface.hpp>
#include <objects/key_value_object.hpp>
#include <objects/account_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <abi_serializer.hpp>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <fc/utf8.hpp>
#if WIN32
#include <fc/int128.hpp>
#endif

#include "jsvm_util.h"
#include "jsvm_objbind/Int64Bind.h"
/*
	void exportFoo(const FunctionCallbackInfo<v8::Value>& args) {
	
		for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope(args.GetIsolate());

		Handle<v8::Value> js_data_value = args[i];
	
		bool bIsObject = js_data_value->IsObject();
		if (bIsObject)
		{
			Handle<Object> js_data_object = Handle<Object>::Cast(js_data_value);

			Handle<String> js_func_name = String::NewFromUtf8(
				Xmaxplatform::Chain::jsvm_xmax::get().current_state->current_isolate,
				"toOctetString",
				NewStringType::kNormal).ToLocalChecked();

			Handle<String> js_param_name = String::NewFromUtf8(
				Xmaxplatform::Chain::jsvm_xmax::get().current_state->current_isolate,
				" ",
				NewStringType::kNormal).ToLocalChecked();

			Handle<Value>  js_func_ref = js_data_object->Get(js_func_name);
			Handle<Function> js_func = Handle<Function>::Cast(js_func_ref);
 
 			Handle<v8::Value> args[1];
 			args[0] = js_param_name;
 
 			Handle<Value> result = js_func->Call(js_data_object, 1, args);
 
 			String::Utf8Value utf8(result);
 			printf("%s\n", *utf8);
		
		}
	}
}
*/
namespace Xmaxplatform {

	namespace Chain {

		jsvm_xmax::jsvm_xmax() {

		}

		jsvm_xmax::key_type jsvm_xmax::to_key_type(const Basetypes::type_name& type_name)
		{
			if ("str" == type_name)
				return str;
			if ("i64" == type_name)
				return i64;
			if ("i128i128" == type_name)
				return i128i128;
			if ("i64i64i64" == type_name)
				return i64i64i64;

			return invalid_key_type;
		}

		std::string jsvm_xmax::to_type_name(key_type key_type)
		{
			switch (key_type)
			{
			case str:
				return "str";
			case i64:
				return "i64";
			case i128i128:
				return "i128i128";
			case i64i64i64:
				return "i64i64i64";
			default:
				return std::string("<invalid key type - ") + boost::lexical_cast<std::string>(int(key_type)) + ">";
			}
		}

		jsvm_xmax& jsvm_xmax::get() {
			static jsvm_xmax*  jsvm = nullptr;
			if (!jsvm)
			{
				//

				jsvm = new jsvm_xmax();
				jsvm->InitV8();
			}
			return *jsvm;
	  }

#ifdef NDEBUG
		const int CHECKTIME_LIMIT = 3000;
#else
		const int CHECKTIME_LIMIT = 36000;
#endif

		void jsvm_xmax::init(message_context_xmax& c)
		{
			try {
				current_validate_context = &c;
				current_precondition_context = &c;
				current_message_context = &c;
				checktime_limit = CHECKTIME_LIMIT;

				load(c.code, c.db);
				const auto& recipient = c.db.get<account_object, by_name>(c.code);
				
				vm_onInit((char*)recipient.code.data());

			} FC_CAPTURE_AND_RETHROW()
		}

		void jsvm_xmax::InitV8()
		{
			V8::InitializeICUDefaultLocation("");
			V8::InitializeExternalStartupData("");
			v8::Platform* platform = platform::CreateDefaultPlatform();
			V8::InitializePlatform(platform);
			V8::Initialize();
		}

		void  jsvm_xmax::vm_apply(char* code) {
		
			Handle<String> js_func_name = String::NewFromUtf8(current_state->current_isolate, "init", NewStringType::kNormal).ToLocalChecked();
			Handle<v8::Value>  js_func_val = current_state->current_context->Global()->Get(js_func_name);
			if (!js_func_val->IsFunction())
			{
				std::cerr << "Can't find js funcion init()" << std::endl;
			}
			Handle<Function> js_func = Handle<Function>::Cast(js_func_val);

			Handle<v8::Value> hResult = js_func->Call(current_state->current_context->Global(), 0, nullptr);
		
		}

		void vm_init(const HandleScope& scope, const Local<ObjectTemplate>& global, const Local<Context>& context, const Context::Scope& ctxScope)
		{
			Isolate* isolate = jsvm_xmax::get().current_state->current_isolate;

			message_context_xmax& msg_contxt = *jsvm_xmax::get().current_message_context;
			const auto& recipient = msg_contxt.db.get<account_object, by_name>(msg_contxt.code);

			CompileJsCode(isolate, context, (char*)recipient.code.data());
			
			message_context_xmax & validate_context = *jsvm_xmax::get().current_validate_context;
 			Handle<v8::Value> params[2];
 			params[0] = I64Cpp2JS(isolate, context, uint64_t(validate_context.msg.code));
 			params[1] = I64Cpp2JS(isolate, context, uint64_t(validate_context.msg.type));
			CallJsFoo(isolate, context,"init", 0, NULL);
		}

		void  jsvm_xmax::vm_onInit(char* code)
		{
			try {
				namespace  ph = std::placeholders;
				EnterJsContext(current_state->current_isolate, std::bind(&vm_init, ph::_1, ph::_2, ph::_3, ph::_4));

				/*
				//const auto& recipient = db.get<account_object, by_name>(name);
				HandleScope current_handle_scope(current_state->current_isolate);

				v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(current_state->current_isolate);
				// Bind the global 'print' function to the C++ Print callback.  
				global->Set(
					v8::String::NewFromUtf8(current_state->current_isolate, "exportFoo", v8::NewStringType::kNormal)
					.ToLocalChecked(),
					v8::FunctionTemplate::New(current_state->current_isolate, exportFoo));

				
				Local<Context> context = Context::New(current_state->current_isolate, NULL, global);

				// Enter the context for compiling and running the hello world script.
				Context::Scope context_scope(context);
				

				const char* jscode = code;

				// Create a string containing the JavaScript source code.
				Local<String> source =
					String::NewFromUtf8(current_state->current_isolate, jscode,
						NewStringType::kNormal).ToLocalChecked();

				// Compile the source code.

				MaybeLocal<Script> temp = Script::Compile(context, source);
				if (temp.IsEmpty())
				{
					std::cerr << "js compile failed" << std::endl;
				}
				Local<Script> script = temp.ToLocalChecked();
				//state.current_script = script;
				if (script.IsEmpty()) {
					std::cerr << "js compile failed" << std::endl;
				}
				//运行脚本代码
				script->Run();
				
				Handle<String> js_data = String::NewFromUtf8(current_state->current_isolate, "Int64", NewStringType::kNormal).ToLocalChecked();
				Handle<v8::Value> js_data_value = context->Global()->Get(js_data);

				bool bIsObject = js_data_value->IsObject();
				if (bIsObject)
				{
					Handle<Object> js_data_object = Handle<Object>::Cast(js_data_value);

					uint64_t  pcode = uint64_t(current_validate_context->msg.code);
					int* pcodes = (int*)&pcode;

					Handle<v8::Value>  argcodev[2];
					argcodev[0] = Int32::New(current_state->current_isolate, pcodes[1]);
					argcodev[1] = Int32::New(current_state->current_isolate, pcodes[0]);

					Handle<v8::Value> codeObj = js_data_object->CallAsConstructor(2, argcodev);

					uint64_t  ptype = uint64_t(current_validate_context->msg.type);
					int* ptypes = (int*)&ptype;

					Handle<v8::Value>  argtypev[2];
					argtypev[0] = Int32::New(current_state->current_isolate, ptypes[0]);
					argtypev[1] = Int32::New(current_state->current_isolate, ptypes[1]);

					Handle<v8::Value> typeObj = js_data_object->CallAsConstructor(2, argtypev);

					Handle<String> js_func_name = String::NewFromUtf8(current_state->current_isolate, "init", NewStringType::kNormal).ToLocalChecked();
					Handle<v8::Value>  js_func_val = context->Global()->Get(js_func_name);
					if (!js_func_val->IsFunction())
					{
						std::cerr << "Can't find js funcion init()" << std::endl;
					}
					else
					{
						Handle<v8::Value> initargs[2];
						initargs[0] = codeObj;
						initargs[1] = typeObj;
						Handle<Function> js_func = Handle<Function>::Cast(js_func_val);
						Handle<v8::Value> hResult = js_func->Call(context->Global(), 2, initargs);
					}
				}*/
			}
			catch (const Runtime::Exception& e) {
				edump((std::string(describeExceptionCause(e.cause))));
				edump((e.callStack));
				throw;
			}
		}

		void jsvm_xmax::load(const account_name& name, const Basechain::database& db)
		{
			const auto& recipient = db.get<account_object, by_name>(name);
			

			auto& state = instances[name];
			if (state.code_version != recipient.code_version) {
 				
 				state.code_version = fc::sha256();
				state.table_key_types.clear();

				try
				{
					const auto init_time = fc::time_point::now();
					if (state.current_isolate == nullptr)
					{
						Isolate::CreateParams create_params;
						create_params.array_buffer_allocator =
							v8::ArrayBuffer::Allocator::NewDefaultAllocator();
						Isolate* isolate = Isolate::New(create_params);
						state.current_isolate = isolate;
						state.current_isolate_scope = new Isolate::Scope(isolate);
					}

					//init abi
					Basetypes::abi abi;
					if (Basetypes::abi_serializer::to_abi(recipient.abi, abi))
					{
						state.tables_fixed = true;
						for (auto& table : abi.tables)
						{
							const auto key_type = to_key_type(table.index_type);
							if (key_type == invalid_key_type)
								throw Serialization::FatalSerializationException("For code \"" + (std::string)name + "\" index_type of \"" +
									table.index_type + "\" referenced but not supported");

							state.table_key_types.emplace(std::make_pair(table.table_name, key_type));
						}
					}
				}
				catch (Serialization::FatalSerializationException exception)
				{
					std::cerr << "Error deserializing WebAssembly binary file:" << std::endl;
					std::cerr << exception.message << std::endl;
					throw;
				}
				//current_module = state.instance;
				//current_memory = getDefaultMemory(current_module);
				current_state = &state;
				table_key_types = &state.table_key_types;
				tables_fixed = state.tables_fixed;
				table_storage = 0;
			}
		}

	}
}

#endif

