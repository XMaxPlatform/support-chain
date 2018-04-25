/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <chain_time.hpp>
#include <blockchain_config.hpp>

namespace Xmaxplatform { namespace Chain {

	const chain_timestamp chain_timestamp::zero_timestamp;

    chain_timestamp chain_timestamp::from(fc::time_point time_val)
    {
        uint64_t us = time_val.time_since_epoch().count();
        uint64_t stamp_us = us - Config::chain_timestamp_epoch_us;

        uint64_t stamp = stamp_us / Config::chain_timestamp_unit_us;

        return chain_timestamp((chain_timestamp::stamp_type) stamp);
    }

	chain_timestamp chain_timestamp::create(stamp_type stamp_val)
	{
		return chain_timestamp(stamp_val);
	}

    fc::time_point chain_timestamp::to_time_point(const chain_timestamp& val)
    {
        int64_t us = ((int64_t)val._stamp) * (int64_t)Config::chain_timestamp_unit_us;

		us += Config::chain_timestamp_epoch_us;

        return fc::time_point(fc::microseconds(us));
    }


}
}


namespace fc {

    void to_variant(const Xmaxplatform::Chain::chain_timestamp& t, fc::variant& v) {
        to_variant( Xmaxplatform::Chain::chain_timestamp::to_time_point(t), v);
    }


    void from_variant(const fc::variant& v, Xmaxplatform::Chain::chain_timestamp& t) {
        t = Xmaxplatform::Chain::chain_timestamp::from(v.as<fc::time_point>());
    }
}