file(GLOB HEADERS "include/*.hpp")
add_library( blockchain_plugin
        blockchain_plugin.cpp
             ${HEADERS} )

target_link_libraries( blockchain_plugin xmaxchain baseapp )
target_include_directories( blockchain_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
