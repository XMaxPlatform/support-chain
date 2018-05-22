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

		void  jsvm_xmax::vm_onInit(char* code)
		{
			try {
				try {
					//const auto& recipient = db.get<account_object, by_name>(name);

					HandleScope current_handle_scope(current_state->current_isolate);
					Local<Context> context = Context::New(current_state->current_isolate);
					

					// Enter the context for compiling and running the hello world script.
					Context::Scope context_scope(context);
					
					const char* jscode = code;

					// Create a string containing the JavaScript source code.
					Local<String> source =
						String::NewFromUtf8(current_state->current_isolate, jscode,
							NewStringType::kNormal).ToLocalChecked();

					// Compile the source code.
					Local<Script> script = Script::Compile(context, source).ToLocalChecked();
					//state.current_script = script;
					if (script.IsEmpty()) {
						std::cerr << "js compile failed" << std::endl;
					}
					//运行脚本代码
					script->Run();
				
					Handle<String> js_func_name = String::NewFromUtf8(current_state->current_isolate , "init", NewStringType::kNormal).ToLocalChecked();
					Handle<v8::Value>  js_func_val = context->Global()->Get(js_func_name);
					if (!js_func_val->IsFunction())
					{
						std::cerr << "Can't find js funcion init()" << std::endl;
					}
					else
					{
						Handle<Function> js_func = Handle<Function>::Cast(js_func_val);
						Handle<v8::Value> hResult = js_func->Call(context->Global(), 0, nullptr);
					}
					
				}
				catch (const Runtime::Exception& e) {
					edump((std::string(describeExceptionCause(e.cause))));
					edump((e.callStack));
					throw;
				}
			} FC_CAPTURE_AND_RETHROW()
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

					Isolate::CreateParams create_params;
					create_params.array_buffer_allocator =
						v8::ArrayBuffer::Allocator::NewDefaultAllocator();
					Isolate* isolate = Isolate::New(create_params);
					state.current_isolate = isolate;
					state.current_isolate_scope = new Isolate::Scope(isolate);



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

