/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <chain_utils.hpp>

namespace Xmaxplatform {
namespace Chain {

	namespace utils
	{
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
	}
}
}