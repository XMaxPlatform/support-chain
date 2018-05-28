
#pragma once
#include <xmaxlib/event.h>


namespace Xmaxplatform {


   /**
    *
    *  This method fires event to output.
    *
    *  @brief Fire event to transaction output
    *  
    *  Example:
    *  @code
    *  struct dummy_event {
    *    char a; //1
    *    unsigned long long b; //8
    *    int  c; //4
    *  };
    *  dummy_event data(...);
    *  fire_event(N(dummy), data);
    *  @endcode
    */
   template<typename T>
   void fire_event(event_name name, const T& event)
   {
       fire_event_internal(name, (void*)&event, sizeof(T));
   }



 ///@} eventcpp api

} // namespace xmax
