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

   uint32_t block_header::num_from_id(const xmax_type_block_id& id)
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

   xmax_type_summary merkle(vector<xmax_type_summary> ids) {
      while (ids.size() > 1) {
         if (ids.size() % 2)
            ids.push_back(ids.back());
         for (int i = 0; i < ids.size() / 2; ++i)
            ids[i/2] = xmax_type_summary::hash(std::make_pair(ids[i], ids[i+1]));
         ids.resize(ids.size() / 2);
      }

      return ids.front();
   }

   xmax_type_merkle_root signed_block::calculate_merkle_root()const
   {
      if(threads.empty())
         return xmax_type_merkle_root();

      vector<xmax_type_summary> ids;
      for (const auto& cycle : threads)
         for (const auto& thread : cycle)
            ids.emplace_back(thread.merkle_digest());

      return xmax_type_merkle_root::hash(merkle(ids));
   }

   xmax_type_summary thread::merkle_digest() const {
      vector<xmax_type_summary> ids;


      return merkle(ids);
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
