

file(GLOB SOURCE_FILES "*.cpp")
file(GLOB HEADERS "include/*.hpp")




add_executable(chain_test ${SOURCE_FILES} ${HEADERS})

target_include_directories(chain_test PUBLIC 
                            ${Boost_INCLUDE_DIR})

target_link_libraries(chain_test 
                    xmaxchain fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS}
                    ${Boost_LIBRARIES})

set_target_properties(chain_test PROPERTIES 
                        FOLDER
                        "Test" )

install(TARGETS 
chain_test 

RUNTIME DESTINATION tests 
LIBRARY DESTINATION tests/lib 
ARCHIVE DESTINATION tests/lib
)