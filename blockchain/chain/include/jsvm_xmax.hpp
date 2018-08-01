#pragma once
#include <blockchain_exceptions.hpp>
#include <message_xmax.hpp>
#include <message_context_xmax.hpp>
#include <libplatform/libplatform.h>
#include <v8.h>
#include "jsvm_objbind/V8Bind.h"
#include "jsvm_util.hpp"

namespace Xmaxplatform {
	namespace Chain {
		class jsvm_xmax {
		public:
			enum key_type {
				str,
				i128,
				i128i128,
				//i64i64i64,
				i128i128i128,
				invalid_key_type
			};
			typedef map<name, key_type> TableMap;
			struct ModuleState {
				//one contract one context map.
				v8::Local<v8::Script>			 current_script;		
				PersistentCpyableContext           current_context;
			
				fc::sha256               code_version;
				TableMap                 table_key_types;
				bool                     tables_fixed = false;
			};

			static jsvm_xmax& get();

			void V8SetInstructionCallBack(const char* name,void* foo);
			void V8SetupGlobalObjTemplate(v8::Local<v8::ObjectTemplate>* pGlobalTemp);
			void V8EnvInit();
			void V8EnvDiscard();
			void V8ExitContext();
			void V8SetBind(V8Bind* bind);
			v8::Isolate* V8GetIsolate();

			bool StoreInstruction(int ins);
			int GetExecutedInsCount();
			std::list<int>& GetExecutedIns();

			void init(message_context_xmax& c);
			void apply(message_context_xmax& c, uint32_t execution_time, bool received_block);
			void validate(message_context_xmax& c);
			void precondition(message_context_xmax& c);

			void LoadScript(account_name name, const char* code, const mapped_vector<char>& abi, const fc::sha256& code_version);
			void LoadScriptTest(account_name name, const char* code, const std::vector<char>& abi, const fc::sha256& code_version, bool sciptTest = false);
			void SetInstructionLimit(uint32_t instructionLimit);

			void  vm_apply();
			void  vm_onInit();

			void vm_calli64param2(const char* foo,uint128 code, uint128 type);

			int64_t current_execution_time();

			void checktime();

			static key_type to_key_type(const Basetypes::type_name& type_name);
			static std::string to_type_name(key_type key_type);

			message_context_xmax*       current_message_context = nullptr;
			message_context_xmax*       current_validate_context = nullptr;
			message_context_xmax*       current_precondition_context = nullptr;

			ModuleState*               current_state = nullptr;
			TableMap*                  table_key_types = nullptr;
			bool                       tables_fixed = false;
			int64_t                    table_storage = 0;

			uint32_t                   checktime_limit = 0;

			int32_t                    per_code_account_max_db_limit_mbytes = Config::default_per_code_account_max_db_limit_mbytes;
			uint32_t                   row_overhead_db_limit_bytes = Config::default_row_overhead_db_limit_bytes;

		private:
			
			void load(const account_name& name, const Basechain::database& db);
			void  vm_validate();
			void  vm_precondition();

			void CleanInstruction();

			map<account_name, ModuleState> instances;
			fc::time_point checktimeStart;

			uint32_t m_instructionCount;
			std::list<int> m_Intrunctions;
			uint32_t m_instructionLimit;

			v8::Isolate* m_pIsolate;
			v8::Local<v8::ObjectTemplate>* m_pGlobalObjectTemplate;

			v8::Isolate::CreateParams m_CreateParams;
			v8::Platform* m_pPlatform;

			V8Bind* m_pBind;

			jsvm_xmax();
		};
		inline v8::Isolate* jsvm_xmax::V8GetIsolate()
		{
			return m_pIsolate;
		}
		inline int jsvm_xmax::GetExecutedInsCount()
		{
			return m_instructionCount;
		}

		inline std::list<int>& jsvm_xmax::GetExecutedIns()
		{
			return m_Intrunctions;
		}

		inline void jsvm_xmax::SetInstructionLimit(uint32_t instructionLimit)
		{
			m_instructionLimit = instructionLimit;
		}
	}
}
