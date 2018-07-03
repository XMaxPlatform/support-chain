add_executable( xmaxwallet main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

target_include_directories(xmaxwallet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries( xmaxwallet
        PRIVATE baseapp		
		PRIVATE chainhttp_plugin wallet_plugin
        PRIVATE xmaxchain fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )

set_target_properties( xmaxwallet PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "8.Launch")

install( TARGETS 
xmaxwallet

RUNTIME DESTINATION bin 
LIBRARY DESTINATION lib 
ARCHIVE DESTINATION lib 
)