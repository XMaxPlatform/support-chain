/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>
#include "multi_index_includes.hpp"


namespace Xmaxplatform { namespace Chain {

   class account_contract {
   public:
	   template<typename _Any_alloc>
	   account_contract(_Any_alloc al)
		   : code(al)
		   , abi(al)
	   {

	   }
	   account_contract(const account_contract& other)
		   : code(other.code.get_allocator())
		   , abi(other.abi.get_allocator())
	   {
		   assign(other);
	   }

	   account_contract& operator = (const account_contract& other)
	   {
		   assign(other);
		   return *this;
	   }

	   void assign(const account_contract& other)
	   {
		   code_version = other.code_version;
		   code.assign(other.code.begin(), other.code.end());
		   abi.assign(other.abi.begin(), other.abi.end());
	   }

	   fc::sha256          code_version;
	   mapped_vector<char> code;
	   mapped_vector<char> abi;

	  void set_abi(const Xmaxplatform::Basetypes::abi& _abi);
   };

} } // Xmaxplatform::chain

FC_REFLECT(Xmaxplatform::Chain::account_contract, (code_version)(code)(abi))
