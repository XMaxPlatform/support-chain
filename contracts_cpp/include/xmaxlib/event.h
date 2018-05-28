
#pragma once
#include <xmaxlib/core.h>

extern "C" {
   
   /**
    *  Fire event with event struct data
    *  @brief Copy current event to the specified location
    *  @param name - event name
    *  @param event - event data copyed from
    *  @param len - event data length
    */
    void fire_event_internal(event_name name, void* event, uint32_t len);
  
}
