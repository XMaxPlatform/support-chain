add_executable( xmaxrun main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

target_include_directories(xmaxrun PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries( xmaxrun
        PRIVATE baseapp
        PRIVATE blockchain_plugin blockbuilder_plugin
        PRIVATE xmaxchain fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )

set_target_properties( xmaxrun PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "8.Launch")