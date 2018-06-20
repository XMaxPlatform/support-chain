#pragma once

#include <xmaxtypes.hpp>

namespace Xmaxplatform {

    class basemisc
    {
    public:
        static const Basetypes::name empty_name_type;
        static const Basetypes::public_key empty_public_key_type;
		static const fc::sha256 empty_sha256;
    };



}

#define empty_name Xmaxplatform::basemisc::empty_name_type

#define empty_public_key Xmaxplatform::basemisc::empty_public_key_type

#define size_mb (1024 * 1024)


#define HAS_BIT_FLAG(_flag, _key) ((_flag & _key) != 0)

#define NO_BIT_FLAG(_flag, _key) ((_flag & _key) == 0)