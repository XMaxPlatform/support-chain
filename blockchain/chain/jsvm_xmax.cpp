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
#include <objects/xmx_token_object.hpp>
#include <abi_serializer.hpp>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <fc/utf8.hpp>
#include <objects/account_object.hpp>
#if WIN32
#include <fc/int128.hpp>
#endif


#include "jsvm_objbind/Int64Bind.h"


using namespace v8;
namespace Xmaxplatform {

	namespace Chain {

		jsvm_xmax::jsvm_xmax() 
			:current_validate_context(nullptr)
			,m_instructionLimit(1000)
		{
			CleanInstruction();
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
				jsvm = new jsvm_xmax();
			}
			return *jsvm;
	   }

#ifdef NDEBUG
		const int CHECKTIME_LIMIT = 3000;
#else
		const int CHECKTIME_LIMIT = 36000;
#endif

		bool jsvm_xmax::StoreInstruction(int ins)
		{
			m_instructionCount++;
			m_Intrunctions.push_back(ins);
			if (m_instructionCount > m_instructionLimit)
			{
				return false;
			}
		}

		void jsvm_xmax::CleanInstruction()
		{
			m_instructionCount = 0;
			m_Intrunctions.clear();
		}

		void jsvm_xmax::init(message_context_xmax& c)
		{
			try {
				current_validate_context = &c;
				current_precondition_context = &c;
				current_message_context = &c;
				checktime_limit = CHECKTIME_LIMIT;

				load(c.code, c.db);
				const auto& recipient = c.db.get<account_object, by_name>(c.code);
				
				vm_onInit();

			} FC_CAPTURE_AND_RETHROW()
		}
		
		Object* CallBackCheck(int args_length, Object** args_object, Isolate* isolate) {
			void* arg1 = *(reinterpret_cast<Object**>(reinterpret_cast<intptr_t>(args_object) - 1 * sizeof(int)));
			int value = (int)arg1;
			HandleScope scope(isolate);
			if (!jsvm_xmax::get().StoreInstruction(value))
				return V8_ThrowException(isolate, "ScriptRunout");

			return args_object[0];
		}

		void jsvm_xmax::V8SetInstructionCallBack(const char* name, void* foo)
		{
			V8_AddIntrinsicFoo(name, foo, 2, 1);
		}

		void jsvm_xmax::V8SetupGlobalObjTemplate(v8::Local<v8::ObjectTemplate>* pGlobalTemp)
		{
			m_pGlobalObjectTemplate = pGlobalTemp;
			BindJsFoos(m_pIsolate, *m_pGlobalObjectTemplate, FooBind::GetBindFoos(m_pIsolate));
			SetupV8i64ObjectToJs(m_pIsolate, *m_pGlobalObjectTemplate);
		}

		void jsvm_xmax::V8EnvInit()
		{
			V8_AddIntrinsicFoo("callback", (void*)CallBackCheck, 2, 1);
			V8::InitializeICUDefaultLocation("");
			V8::InitializeExternalStartupData("");
			m_pPlatform = platform::CreateDefaultPlatform();
			V8::InitializePlatform(m_pPlatform);
			V8::Initialize();

			m_CreateParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
			m_pIsolate = v8::Isolate::New(m_CreateParams);
		}


		void jsvm_xmax::V8EnvDiscard()
		{
			m_pIsolate->Dispose();
			v8::V8::Dispose();
			v8::V8::ShutdownPlatform();
			delete m_pPlatform;
			delete m_CreateParams.array_buffer_allocator;
		}

		void jsvm_xmax::V8ExitContext()
		{
			if (current_state != NULL && !current_state->current_context.IsEmpty())
			{
				current_state->current_context.Get(m_pIsolate)->Exit();
			}
		}

		void  jsvm_xmax::vm_apply() {
			message_context_xmax* p_validate_context = jsvm_xmax::get().current_validate_context;

			uint64_t code = 0;
			uint64_t type = 0;
			if (p_validate_context != nullptr)
			{
				code = uint64_t(p_validate_context->msg.code);
				type = uint64_t(p_validate_context->msg.type);
			}
			vm_calli64param2("apply", code, type);
		}

		void  jsvm_xmax::vm_onInit()
		{
			message_context_xmax* p_validate_context = jsvm_xmax::get().current_validate_context;
			
			uint64_t code = 0;
			uint64_t type = 0;
			if (p_validate_context != nullptr)
			{
				code = uint64_t(p_validate_context->msg.code);
				type = uint64_t(p_validate_context->msg.type);
			}
			vm_calli64param2("init", code, type);
		}

		void jsvm_xmax::vm_calli64param2(const char* foo, uint64_t code, uint64_t type)
		{
			try {
				CleanInstruction();
				Local<Context> context = current_state->current_context.Get(m_pIsolate);
				Handle<v8::Value> params[2];
				params[0] = I64Cpp2JS(m_pIsolate, context, code);
				params[1] = I64Cpp2JS(m_pIsolate, context, type);
				CallJsFoo(m_pIsolate, context, foo, 0, NULL);
			}
			catch (const Runtime::Exception& e) {
				edump((std::string(describeExceptionCause(e.cause))));
				edump((e.callStack));
				throw;
			}
			catch (...)
			{
				edump((std::string("js code run out")));
				throw;
			}
		}

		void jsvm_xmax::load(const account_name& name, const Basechain::database& db)
		{
			const auto& accountobj = db.get<account_object, by_name>(name);

			message_context_xmax& msg_contxt = *jsvm_xmax::get().current_message_context;
			const auto& recipient = msg_contxt.db.get<account_object, by_name>(msg_contxt.code);
			LoadScript(name, recipient.code.data(), accountobj.abi, accountobj.code_version);
		}

		void jsvm_xmax::LoadScript(account_name name, const char* code,const mapped_vector<char>& abi,const fc::sha256& code_version)
		{
			auto& state = instances[name];

			if (state.code_version != code_version) {

				state.code_version = code_version;
				state.table_key_types.clear();

				if (!state.current_context.IsEmpty())
					state.current_context.Reset();

				state.current_context = CreateJsContext(m_pIsolate, *m_pGlobalObjectTemplate);

				V8ExitContext();

				EnterJsContext(m_pIsolate, state.current_context);

				state.current_script = CompileJsCode(m_pIsolate, state.current_context.Get(m_pIsolate), (char*)code);
				current_state = &state;
			
				try
				{
					const auto init_time = fc::time_point::now();

					//init abi
					Basetypes::abi byteabi;
					if (Basetypes::abi_serializer::to_abi(abi, byteabi))
					{
						state.tables_fixed = true;
						for (auto& table : byteabi.tables)
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
					std::cerr << "Error:" << std::endl;
					std::cerr << exception.message << std::endl;
					throw;
				}

				table_key_types = &state.table_key_types;
				tables_fixed = state.tables_fixed;
				table_storage = 0;

			}
			else if (current_state != &state)
			{
				if (!current_state->current_context.IsEmpty())
					ExitJsContext(m_pIsolate, current_state->current_context);

				EnterJsContext(m_pIsolate, state.current_context);
				current_state = &state;
			}
		}

		void jsvm_xmax::LoadScriptTest(account_name name, const char* code, const std::vector<char>& abi, const fc::sha256& code_version, bool sciptTest /*= false*/)
		{
			auto& state = instances[name];

			if (state.code_version != code_version) {

				state.code_version = fc::sha256();
				state.table_key_types.clear();

				if (!state.current_context.IsEmpty())
					state.current_context.Reset();

				state.current_context = CreateJsContext(m_pIsolate, *m_pGlobalObjectTemplate);

				V8ExitContext();

				EnterJsContext(m_pIsolate, state.current_context);

				state.current_script = CompileJsCode(m_pIsolate, state.current_context.Get(m_pIsolate), (char*)code);
				current_state = &state;
				if (sciptTest)
				{

				}
				else
				{
					try
					{
						const auto init_time = fc::time_point::now();

						//init abi
						Basetypes::abi byteabi;
						if (Basetypes::abi_serializer::to_abi(abi, byteabi))
						{
							state.tables_fixed = true;
							for (auto& table : byteabi.tables)
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
						std::cerr << "Error:" << std::endl;
						std::cerr << exception.message << std::endl;
						throw;
					}

					table_key_types = &state.table_key_types;
					tables_fixed = state.tables_fixed;
					table_storage = 0;

				}

			}
			else if (current_state != &state)
			{
				if (!current_state->current_context.IsEmpty())
					ExitJsContext(m_pIsolate, current_state->current_context);

				EnterJsContext(m_pIsolate, state.current_context);
				current_state = &state;
			}
		}

	}
}

#endif

