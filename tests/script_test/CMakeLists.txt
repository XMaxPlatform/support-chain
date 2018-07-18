

file(GLOB SOURCE_FILES "*.cpp")
file(GLOB HEADERS "include/*.hpp")




add_executable(script_test ${SOURCE_FILES} ${HEADERS})

target_include_directories(script_test PUBLIC 
                            ${Boost_INCLUDE_DIR})

target_link_libraries(script_test 
                   PRIVATE xmaxchain fc jsbind_plugin blockchain_plugin
 ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS}
                    ${Boost_LIBRARIES})
if(USE_V8_VM)
target_link_libraries( script_test
      PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8.lib
	  PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8_libbase.lib
      PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8_libplatform.lib
      PRIVATE debug  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/debug/v8_libsampler.lib
 )

target_link_libraries( script_test
      PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8.lib
	  PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8_libbase.lib
      PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8_libplatform.lib
      PRIVATE optimized  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/library/release/v8_libsampler.lib
 )
 endif()	

set_target_properties(script_test PROPERTIES 
                        FOLDER
                        "Test" )

install(TARGETS 
script_test 

RUNTIME DESTINATION tests 
LIBRARY DESTINATION tests/lib 
ARCHIVE DESTINATION tests/lib
)