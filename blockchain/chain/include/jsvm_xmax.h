#pragma once
#pragma once
#include <blockchain_exceptions.hpp>
#include <message_xmax.hpp>
#include <message_context_xmax.hpp>

#include <libplatform/libplatform.h>
#include <v8.h>


using namespace v8;

namespace Xmaxplatform {
	namespace Chain {
		class jsvm_xmax {
		public:
			enum key_type {
				str,
				i64,
				i128i128,
				i64i64i64,
				invalid_key_type
			};
			typedef map<name, key_type> TableMap;
			struct ModuleState {
				

				Local<Script>			 current_script;
				Isolate*                 current_isolate = nullptr;
				Local<Context>           current_context;

				Isolate::Scope*          current_isolate_scope;
				HandleScope*             current_handle_scope;	
				Context::Scope*			 current_context_scope;

				fc::sha256               code_version;
				TableMap                 table_key_types;
				bool                     tables_fixed = false;
			};

			static jsvm_xmax& get();

			void init(message_context_xmax& c);
			void apply(message_context_xmax& c, uint32_t execution_time, bool received_block);
			void validate(message_context_xmax& c);
			void precondition(message_context_xmax& c);

			int64_t current_execution_time();

			void checktime();

			static key_type to_key_type(const Basetypes::type_name& type_name);
			static std::string to_type_name(key_type key_type);

			message_context_xmax*       current_message_context = nullptr;
			message_context_xmax*       current_validate_context = nullptr;
			message_context_xmax*       current_precondition_context = nullptr;

// 			Runtime::MemoryInstance*   current_memory = nullptr;
// 			Runtime::ModuleInstance*   current_module = nullptr;
// 			wasm_memory*               current_memory_management = nullptr;
			ModuleState*               current_state = nullptr;
			TableMap*                  table_key_types = nullptr;
			bool                       tables_fixed = false;
			int64_t                    table_storage = 0;

			uint32_t                   checktime_limit = 0;

			int32_t                    per_code_account_max_db_limit_mbytes = Config::default_per_code_account_max_db_limit_mbytes;
			uint32_t                   row_overhead_db_limit_bytes = Config::default_row_overhead_db_limit_bytes;

		private:
			void InitV8();
			void load(const account_name& name, const Basechain::database& db);
			void  vm_validate();
			void  vm_precondition();
			void  vm_apply(char* code);
			void  vm_onInit(char* code);
			//U32   vm_pointer_to_offset(char*);



			map<account_name, ModuleState> instances;
			fc::time_point checktimeStart;

			jsvm_xmax();
		};
	}
}
