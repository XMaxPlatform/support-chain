#pragma once

#include <xmaxtypes.hpp>

namespace Xmaxplatform {

    class basemisc
    {
    public:
        static const Basetypes::name empty_name_type;
        static const Basetypes::public_key empty_public_key_type;
    };



}

#define empty_name Xmaxplatform::basemisc::empty_name_type

#define empty_public_key Xmaxplatform::basemisc::empty_public_key_type

#define size_mb (1024 * 1024)