add_executable( xmaxrun main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

target_include_directories(xmaxrun PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


if(WIN32)
	list(APPEND PLATFORM_SPECIFIC_LIBS 
		${Secp256k1_LIBRARY_DIR}/gmp.lib
		${Secp256k1_LIBRARY_DIR}/secp256k1.lib
		${OPENSSL_LIBRARY_DIR}/libcrypto.lib
		${OPENSSL_LIBRARY_DIR}/openssl.lib
		Userenv
	)
endif()


target_link_libraries( xmaxrun
        PRIVATE baseapp
        PRIVATE blockchain_plugin blockbuilder_plugin chainhttp_plugin chainnet_plugin mongodb_plugin contractutil_plugin
        PRIVATE xmaxchain fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )
if(USE_V8_VM)
target_link_libraries( xmaxrun
      PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8.lib
	  PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8_libbase.lib
      PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8_libplatform.lib
      PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8_libsampler.lib
 )

target_link_libraries( xmaxrun
      PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8.lib
	  PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8_libbase.lib
      PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8_libplatform.lib
      PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8_libsampler.lib
 )
endif()	
set_target_properties( xmaxrun PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "8.Launch")

install( TARGETS 
xmaxrun

RUNTIME DESTINATION bin 
LIBRARY DESTINATION lib 
ARCHIVE DESTINATION lib 
)