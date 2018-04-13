/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <blockchain_types.hpp>

namespace Xmaxplatform { namespace Chain {

    class chain_timestamp
    {
    public:
        typedef uint32_t stamp_type;

        inline fc::time_point time_point() const
        {
            return to_time_point(*this);
        }

        static chain_timestamp from(fc::time_point val);

        static fc::time_point to_time_point(const chain_timestamp& val);


        bool   operator > ( const chain_timestamp& t )const   { return _stamp >  t._stamp; }
        bool   operator >=( const chain_timestamp& t )const   { return _stamp >= t._stamp; }
        bool   operator < ( const chain_timestamp& t )const   { return _stamp <  t._stamp; }
        bool   operator <=( const chain_timestamp& t )const   { return _stamp <= t._stamp; }
        bool   operator ==( const chain_timestamp& t )const   { return _stamp == t._stamp; }
        bool   operator !=( const chain_timestamp& t )const   { return _stamp != t._stamp; }
        
        inline void set_data(const stamp_type & val)
        {
            _stamp = val;
        }

        inline stamp_type get_data() const
        {
            return _stamp;
        }
        chain_timestamp()
        : _stamp(0)
        {

        }
    public:

        stamp_type _stamp;

    private:
        chain_timestamp(uint32_t stamp)
        : _stamp(stamp)
        {

        }
    };



}
}
#include <fc/reflect/reflect.hpp>
FC_REFLECT(Xmaxplatform::Chain::chain_timestamp, (_stamp))

namespace fc {

    void to_variant(const Xmaxplatform::Chain::chain_timestamp& t, fc::variant& v);


    void from_variant(const fc::variant& v, Xmaxplatform::Chain::chain_timestamp& t);
}