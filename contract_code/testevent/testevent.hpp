#include <xmaxlib/xmax.hpp>

namespace testevent {

    struct PACKED(ontestevent) {
        account_name from;     
        uint64_t numfield;
    };

    struct PACKED(normalaction) {
        account_name testaccount;
        uint64_t abc;
    };


}