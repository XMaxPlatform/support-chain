/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <boost/function.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <vm_xmax.hpp>
#include <chain_xmax.hpp>
#include <vm_native_interface.hpp>
#include <objects/key_value_object.hpp>
#include <objects/account_object.hpp>
#include <objects/contract_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <abi_serializer.hpp>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <fc/utf8.hpp>
#if WIN32
#include <fc/int128.hpp>
#endif

namespace Xmaxplatform { namespace Chain {
   using namespace IR;
   using namespace Runtime;
   typedef boost::multiprecision::cpp_bin_float_50 DOUBLE;
   const uint32_t bytes_per_mbyte = 1024 * 1024;

   class wasm_memory
   {
   public:
      explicit wasm_memory(vm_xmax& vm);
      wasm_memory(const wasm_memory&) = delete;
      wasm_memory(wasm_memory&&) = delete;
      ~wasm_memory();
      U32 sbrk(I32 num_bytes);
   private:
      static U32 limit_32bit_address(Uptr address);

      static const U32 _max_memory = 1024 * 1024;
      vm_xmax& _vm_xmax;
      Uptr _num_pages;
      const U32 _min_bytes;
      U32 _num_bytes;
   };

   // account.h/hpp expected account API balance interchange format
   // must match account.hpp account_balance definition
   PACKED_STRUCT(
   struct account_balance
   {
      /**
      * Name of the account who's balance this is
      */
      account_name account;

      /**
      * Balance for this account
      */
      asset xmx_balance;

      /**
      * Staked balance for this account
      */
      asset staked_balance;

      /**
      * Unstaking balance for this account
      */
      asset unstaking_balance;

      /**
      * Time at which last unstaking occurred for this account
      */
      time last_unstaking_time;
   })

   vm_xmax::vm_xmax() {
   }

   vm_xmax::key_type vm_xmax::to_key_type(const Basetypes::type_name& type_name)
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

   std::string vm_xmax::to_type_name(key_type key_type)
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

#ifdef NDEBUG
   const int CHECKTIME_LIMIT = 3000;
#else
   const int CHECKTIME_LIMIT = 36000;
#endif

   void vm_xmax::checktime()
   {
	   int64_t duration = current_execution_time();

      if (duration > checktime_limit) {
         wlog("checktime called ${d}", ("d", duration));
         throw checktime_exceeded();
      }
   }


   vm_xmax& vm_xmax::get() {
      static vm_xmax*  wasm = nullptr;
      if( !wasm )
      {
         wlog( "Runtime::init" );
		 vm_native_log();
         Runtime::init();
         wasm = new vm_xmax();
      }
      return *wasm;
   }

   struct RootResolver : Runtime::Resolver
   {
      std::map<std::string,Resolver*> moduleNameToResolverMap;

     bool resolve(const std::string& moduleName,const std::string& exportName,ObjectType type,ObjectInstance*& outObject) override
     {
         // Try to resolve an intrinsic first.
         if(IntrinsicResolver::singleton.resolve(moduleName,exportName,type,outObject)) { return true; }
         FC_ASSERT( !"unresolvable", "${module}.${export}", ("module",moduleName)("export",exportName) );
         return false;
     }
   };

   int64_t vm_xmax::current_execution_time()
   {
      return (fc::time_point::now() - checktimeStart).count();
   }


   char* vm_xmax::vm_allocate( int bytes ) {
      FunctionInstance* alloc_function = asFunctionNullable(getInstanceExport(current_module,"alloc"));
      const FunctionType* functionType = getFunctionType(alloc_function);
      FC_ASSERT( functionType->parameters.size() == 1 );
      std::vector<Value> invokeArgs(1);
      invokeArgs[0] = U32(bytes);

      checktimeStart = fc::time_point::now();

      auto result = Runtime::invokeFunction(alloc_function,invokeArgs);

      return &memoryRef<char>( current_memory, result.i32 );
   }

   U32 vm_xmax::vm_pointer_to_offset( char* ptr ) {
      return U32(ptr - &memoryRef<char>(current_memory,0));
   }

   void  vm_xmax::vm_call( const char* name ) {
   try {
      std::unique_ptr<wasm_memory> wasm_memory_mgmt;
      try {
         /*
         name += "_" + std::string( current_validate_context->msg.code ) + "_";
         name += std::string( current_validate_context->msg.type );
         */
         /// TODO: cache this somehow
         FunctionInstance* call = asFunctionNullable(getInstanceExport(current_module,name) );
         if( !call ) {
            //wlog( "unable to find call ${name}", ("name",name));
            return;
         }
         //FC_ASSERT( apply, "no entry point found for ${call}", ("call", std::string(name))  );

         FC_ASSERT( getFunctionType(call)->parameters.size() == 2 );

  //       idump((current_validate_context->msg.code)(current_validate_context->msg.type)(current_validate_context->code));
         std::vector<Value> args = { Value(uint64_t(current_validate_context->msg.code)),
                                     Value(uint64_t(current_validate_context->msg.type)) };

         auto& state = *current_state;
         char* memstart = &memoryRef<char>( current_memory, 0 );
         memset( memstart + state.mem_end, 0, ((1<<16) - state.mem_end) );
         memcpy( memstart, state.init_memory.data(), state.mem_end);

         checktimeStart = fc::time_point::now();
         wasm_memory_mgmt.reset(new wasm_memory(*this));

         Runtime::invokeFunction(call,args);
         wasm_memory_mgmt.reset();
         checktime();
      } catch( const Runtime::Exception& e ) {
          edump((std::string(describeExceptionCause(e.cause))));
          edump((e.callStack));
          throw;
      }
   } FC_CAPTURE_AND_RETHROW( (name)(current_validate_context->msg.type) ) }

   void  vm_xmax::vm_apply()        { vm_call("apply" );          }

   void  vm_xmax::vm_onInit()
   { try {
      try {
         FunctionInstance* apply = asFunctionNullable(getInstanceExport(current_module,"init"));
         if( !apply ) {
            elog( "no onInit method found" );
            return; /// if not found then it is a no-op
         }

         checktimeStart = fc::time_point::now();

         const FunctionType* functionType = getFunctionType(apply);
         FC_ASSERT( functionType->parameters.size() == 0 );

         std::vector<Value> args(0);

         Runtime::invokeFunction(apply,args);
      } catch( const Runtime::Exception& e ) {
         edump((std::string(describeExceptionCause(e.cause))));
         edump((e.callStack));
         throw;
      }
   } FC_CAPTURE_AND_RETHROW() }

   void vm_xmax::validate( message_context_xmax& c ) {
      /*
      current_validate_context       = &c;
      current_precondition_context   = nullptr;
      current_message_context          = nullptr;

      load( c.code, c.db );
      vm_validate();
      */
   }
   void vm_xmax::precondition( message_context_xmax& c ) {
   try {

      /*
      current_validate_context       = &c;
      current_precondition_context   = &c;

      load( c.code, c.db );
      vm_precondition();
      */

   } FC_CAPTURE_AND_RETHROW() }


   void vm_xmax::apply( message_context_xmax& c, uint32_t execution_time, bool received_block ) {
    try {
      current_validate_context       = &c;
      current_precondition_context   = &c;
      current_message_context          = &c;
      checktime_limit                = execution_time;

      load( c.code, c.db );
      // if this is a received_block, then ignore the table_key_types
      if (received_block)
         table_key_types = nullptr;

      vm_apply();
   } FC_CAPTURE_AND_RETHROW() }

   void vm_xmax::init( message_context_xmax& c ) {
    try {
      current_validate_context       = &c;
      current_precondition_context   = &c;
      current_message_context          = &c;
      checktime_limit                = CHECKTIME_LIMIT;

      load( c.code, c.db );


      vm_onInit();


   } FC_CAPTURE_AND_RETHROW() }



   void vm_xmax::load( const account_name& name, const Basechain::database& db ) {
      const auto& recipient = db.get<contract_object,by_name>( name );
  //    idump(("recipient")(name(name))(recipient.code_version));

      auto& state = instances[name];
      if( state.code_version != recipient.code_version ) {
        if( state.instance ) {
/// TODO: free existing instance and module
//#warning TODO: free existing module if the code has been updated, currently leak memory
           state.instance     = nullptr;
           state.module       = nullptr;
           state.code_version = fc::sha256();
        }
        state.module = new IR::Module();
        state.table_key_types.clear();

        try
        {
//          wlog( "LOADING CODE" );
          const auto start = fc::time_point::now();
          Serialization::MemoryInputStream stream((const U8*)recipient.code.data(),recipient.code.size());
          WASM::serializeWithInjection(stream,*state.module);

          RootResolver rootResolver;
          LinkResult linkResult = linkModule(*state.module,rootResolver);
          state.instance = instantiateModule( *state.module, std::move(linkResult.resolvedImports) );
          FC_ASSERT( state.instance );
          const auto llvm_time = fc::time_point::now();

          current_memory = Runtime::getDefaultMemory(state.instance);

          char* memstart = &memoryRef<char>( current_memory, 0 );
         // state.init_memory.resize(1<<16); /// TODO: actually get memory size
          const auto allocated_memory = Runtime::getDefaultMemorySize(state.instance);
          for( uint64_t i = 0; i < allocated_memory; ++i )
          {
             if( memstart[i] )
             {
                state.mem_end = i+1;
             }
          }
          //ilog( "INIT MEMORY: ${size}", ("size", state.mem_end) );

          state.init_memory.resize(state.mem_end);
          memcpy( state.init_memory.data(), memstart, state.mem_end ); //state.init_memory.size() );
          //std::cerr <<"\n";
          state.code_version = recipient.code_version;
//          idump((state.code_version));
          const auto init_time = fc::time_point::now();

            Basetypes::abi abi;
          if( Basetypes::abi_serializer::to_abi(recipient.abi, abi) )
          {
             state.tables_fixed = true;
             for(auto& table : abi.tables)
             {
                const auto key_type = to_key_type(table.index_type);
                if (key_type == invalid_key_type)
                   throw Serialization::FatalSerializationException("For code \"" + (std::string)name + "\" index_type of \"" +
                                                                    table.index_type + "\" referenced but not supported");

                state.table_key_types.emplace(std::make_pair(table.table_name, key_type));
             }
          }
          ilog("vm_xmax::load name = ${n} times llvm:${llvm} ms, init:${init} ms, abi:${abi} ms",
               ("n",name)("llvm",(llvm_time-start).count()/1000)("init",(init_time-llvm_time).count()/1000)("abi",(fc::time_point::now()-init_time).count()/1000));
        }
        catch(Serialization::FatalSerializationException exception)
        {
          std::cerr << "Error deserializing WebAssembly binary file:" << std::endl;
          std::cerr << exception.message << std::endl;
          throw;
        }
        catch(IR::ValidationException exception)
        {
          std::cerr << "Error validating WebAssembly binary file:" << std::endl;
          std::cerr << exception.message << std::endl;
          throw;
        }
        catch(std::bad_alloc)
        {
          std::cerr << "Memory allocation failed: input is likely malformed" << std::endl;
          throw;
        }
      }
      current_module      = state.instance;
      current_memory      = getDefaultMemory( current_module );
      current_state       = &state;
      table_key_types     = &state.table_key_types;
      tables_fixed        = state.tables_fixed;
      table_storage       = 0;
   }

   wasm_memory::wasm_memory(vm_xmax& vm)
   : _vm_xmax(vm)
   , _num_pages(Runtime::getMemoryNumPages(vm.current_memory))
   , _min_bytes(limit_32bit_address(_num_pages << numBytesPerPageLog2))
   {
      _vm_xmax.current_memory_management = this;
      _num_bytes = _min_bytes;
   }

   wasm_memory::~wasm_memory()
   {
      if (_num_bytes > _min_bytes)
         sbrk((I32)_min_bytes - (I32)_num_bytes);

      _vm_xmax.current_memory_management = nullptr;
   }

   U32 wasm_memory::sbrk(I32 num_bytes)
   {
      const U32 previous_num_bytes = _num_bytes;
      if(Runtime::getMemoryNumPages(_vm_xmax.current_memory) != _num_pages)
         throw Xmaxplatform::Chain::page_memory_error();

      // Round the absolute value of num_bytes to an alignment boundary, and ensure it won't allocate too much or too little memory.
      num_bytes = (num_bytes + 7) & ~7;
      if(num_bytes > 0 && previous_num_bytes > _max_memory - num_bytes)
         throw Xmaxplatform::Chain::page_memory_error();
      else if(num_bytes < 0 && previous_num_bytes < _min_bytes - num_bytes)
         throw Xmaxplatform::Chain::page_memory_error();

      // Update the number of bytes allocated, and compute the number of pages needed for it.
      _num_bytes += num_bytes;
      const Uptr num_desired_pages = (_num_bytes + IR::numBytesPerPage - 1) >> IR::numBytesPerPageLog2;

      // Grow or shrink the memory object to the desired number of pages.
      if(num_desired_pages > _num_pages)
         Runtime::growMemory(_vm_xmax.current_memory, num_desired_pages - _num_pages);
      else if(num_desired_pages < _num_pages)
         Runtime::shrinkMemory(_vm_xmax.current_memory, _num_pages - num_desired_pages);

      _num_pages = num_desired_pages;

      return previous_num_bytes;
   }

   U32 wasm_memory::limit_32bit_address(Uptr address)
   {
      return (U32)(address > UINT32_MAX ? UINT32_MAX : address);
   }

} }



