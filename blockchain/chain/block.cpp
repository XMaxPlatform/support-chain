/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <block.hpp>
#include <fc/io/raw.hpp>
#include <fc/bitutil.hpp>
#include <algorithm>

namespace Xmaxplatform { namespace Chain {
   xmax_type_summary block_header::digest()const
   {
      return xmax_type_summary::hash(*this);
   }

   xmax_type_block_num block_header::num_from_id(const xmax_type_block_id& id)
   {
      return fc::endian_reverse_u32(id._hash[0]);
   }

   xmax_type_block_id signed_block_header::id()const
   {
      xmax_type_block_id result = fc::sha256::hash(*this);
      result._hash[0] &= 0xffffffff00000000;
      result._hash[0] += fc::endian_reverse_u32(block_num()); // store the block num in the ID, 160 bits is plenty for the hash
      return result;
   }

   fc::ecc::public_key signed_block_header::get_signer_key()const
   {
      return fc::ecc::public_key(builder_signature, digest(), true/*enforce canonical*/);
   }

   void signed_block_header::sign(const fc::ecc::private_key& signer)
   {
      builder_signature = signer.sign_compact(digest());
   }

   bool signed_block_header::is_signer_valid(const fc::ecc::public_key &signer_key)const
   {
      return get_signer_key() == signer_key;
   }

   xmax_type_summary transaction_receipt::cal_digest() const
   {
	   xmax_type_summary::encoder coder;

	   fc::raw::pack(coder, receipt_idx);
	   fc::raw::pack(coder, result);

	   if (trx.contains<xmax_type_transaction_id>())
		   fc::raw::pack(coder, trx.get<xmax_type_transaction_id>());
	   else
		   fc::raw::pack(coder, trx.get<transaction_package>().cal_digest());
	   return coder.result();
   }

   xmax_type_summary block_confirmation_header::digest() const
   {
	   return xmax_type_summary::hash(*this);
   }

   fc::ecc::public_key block_confirmation::get_signer_key()const
   {
	   return fc::ecc::public_key(builder_signature, digest(), true/*enforce canonical*/);
   }

   void block_confirmation::sign(const fc::ecc::private_key& signer)
   {
	   builder_signature = signer.sign_compact(digest());
   }

   bool block_confirmation::is_signer_valid(const fc::ecc::public_key &signer_key) const
   {
	   return get_signer_key() == signer_key;
   }



} }
