/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <chain_utils.hpp>
#include <fc/io/raw.hpp>
#include <fc/bitutil.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <algorithm>

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
		xmax_type_block_num num_from_id(const xmax_type_block_id& id)
		{
			return fc::endian_reverse_u32(id._hash[0]);
		}

		xmax_type_block_id block_id(xmax_type_summary summary, xmax_type_block_num num)
		{
			summary._hash[0] &= 0xffffffff00000000;
			summary._hash[0] += fc::endian_reverse_u32(num); // store the block num in the ID, 160 bits is plenty for the hash
			return summary;
		}

		chain_timestamp get_delta_slot_time(chain_timestamp begin_time, uint32_t delta_slot)
		{
			begin_time += chain_timestamp::create(delta_slot);
			return begin_time;

		}

		uint32_t get_delta_slot_at_time(chain_timestamp begin_time, chain_timestamp when)
		{
			chain_timestamp first_slot_time = utils::get_delta_slot_time(begin_time, 1);
			if (when < first_slot_time)
				return 0;

			chain_timestamp sub = when - first_slot_time;
			return sub.get_stamp() + 1;
		}	

		xmax_type_summary cal_merkle(vector<xmax_type_summary> digests) {

			if (0 == digests.size())
			{
				return xmax_type_summary();
			}
			while (digests.size() > 1) {
				if (digests.size() % 2)
					digests.push_back(digests.back());
				for (int i = 0; i < digests.size() / 2; ++i)
					digests[i / 2] = xmax_type_summary::hash(std::make_pair(digests[i], digests[i + 1]));
				digests.resize(digests.size() / 2);
			}

			return digests.front();
		}

		bool is_sign_valid(const xmax_type_signature& sig, const xmax_type_summary& digest, const fc::ecc::public_key &signer_key)
		{
			fc::ecc::public_key pk(sig, digest, true/*enforce canonical*/);

			return pk == signer_key;
		}

		bool is_sign_of_addr(const xmax_type_signature& sig, const xmax_type_summary& digest, const address& addr)
		{
			address addr2 = to_address(sig, digest);

			return addr2 == addr;
		}


		address to_address(const fc::ecc::public_key& pkey)
		{
			xmax_type_summary hash = xmax_type_summary::hash(pkey);

			Basetypes::address_data addr_data;

			memcpy(addr_data.data, hash.data(), 20);

			return address(addr_data);
		}

		address to_address(const xmax_type_signature& sig, const xmax_type_summary& digest)
		{
			fc::ecc::public_key pk(sig, digest, true/*enforce canonical*/);
			return to_address(pk);
		}

	}
}
}