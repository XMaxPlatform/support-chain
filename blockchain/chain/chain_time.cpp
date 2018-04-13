/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <chain_time.hpp>
#include <blockchain_config.hpp>

namespace Xmaxplatform { namespace Chain {

    chain_timestamp chain_timestamp::from(fc::time_point val)
    {
        uint64_t us = val.time_since_epoch().count();
        uint64_t stamp_us = us - Config::chain_timestamp_epoch_us;

        uint64_t stamp = stamp_us / Config::chain_timestamp_unit_us;

        return chain_timestamp((chain_timestamp::stamp_type) stamp);
    }

    fc::time_point chain_timestamp::to_time_point(const chain_timestamp& val)
    {
        int64_t msec = ((int64_t)val._stamp) * (int64_t)Config::chain_timestamp_unit_us;

        msec += Config::chain_timestamp_epoch_us;

        return fc::time_point(fc::milliseconds(msec));
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