#include <testevent.hpp>
#include <xmaxlib/event.hpp>
#include <xmaxlib/message.hpp>

using namespace Xmaxplatform;

namespace testevent {

    void apply_testevent_normal(const testevent::normalaction& info) {
        testevent::ontestevent event;
        event.from = info.testaccount;       
        event.numfield = info.abc;
        fire_event(XNAME(ontestevent), event);
    }
}


using namespace testevent;

extern "C" {
    void init()  {        
        
    }

     void apply( uint64_t code, uint64_t action ) {
        //Dispatch call method accroding to the code and action
        if( code == XNAME(testevent)) {
            if( action == XNAME(normalaction)) {
                apply_testevent_normal(Xmaxplatform::current_message<testevent::normalaction>());                 
            }


        }
    }
}