/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_exceptions.hpp>
#include <message_xmax.hpp>
#include <message_context_xmax.hpp>
#include <IR/Module.h>
#include <Runtime/Runtime.h>
namespace Xmaxplatform { namespace Chain {

class chain_xmax;
class wasm_memory;

/**
 * @class vm_xmax
 *
 * XMAX uses the wasm-jit library to evaluate web assembly code. This library relies
 * upon a singlton interface which means there can be only one instance. This interface
 * is designed to wrap that singlton interface and potentially make it thread-local state
 * in the future.
 */
class vm_xmax {
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
         Runtime::ModuleInstance* instance = nullptr;
         IR::Module*              module = nullptr;
         int                      mem_start = 0;
         int                      mem_end = 1<<16;
         vector<char>             init_memory;
         fc::sha256               code_version;
         TableMap                 table_key_types;
         bool                     tables_fixed = false;
      };

      static vm_xmax& get();

      void init( message_context_xmax& c );
      void apply( message_context_xmax& c, uint32_t execution_time, bool received_block );
      void validate( message_context_xmax& c );
      void precondition( message_context_xmax& c );

      int64_t current_execution_time();

      static key_type to_key_type(const Basetypes::type_name& type_name);
      static std::string to_type_name(key_type key_type);

      message_context_xmax*       current_message_context = nullptr;
      message_context_xmax*       current_validate_context = nullptr;
      message_context_xmax*       current_precondition_context = nullptr;

      Runtime::MemoryInstance*   current_memory = nullptr;
      Runtime::ModuleInstance*   current_module = nullptr;
      ModuleState*               current_state = nullptr;
      wasm_memory*               current_memory_management = nullptr;
      TableMap*                  table_key_types = nullptr;
      bool                       tables_fixed = false;
      int64_t                    table_storage = 0;

      uint32_t                   checktime_limit = 0;

      int32_t                    per_code_account_max_db_limit_mbytes = Config::default_per_code_account_max_db_limit_mbytes;
      uint32_t                   row_overhead_db_limit_bytes = Config::default_row_overhead_db_limit_bytes;

   private:
      void load( const account_name& name, const Basechain::database& db );

      char* vm_allocate( int bytes );   
      void  vm_call( const char* name );
      void  vm_validate();
      void  vm_precondition();
      void  vm_apply();
      void  vm_onInit();
      U32   vm_pointer_to_offset( char* );



      map<account_name, ModuleState> instances;
      fc::time_point checktimeStart;

      vm_xmax();
};


} } // Xmaxplatform::Chain
