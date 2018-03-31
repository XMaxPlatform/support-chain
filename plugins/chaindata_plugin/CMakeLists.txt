file(GLOB HEADERS "include/*.hpp")
add_library( chaindata_plugin
             chaindata_plugin.cpp
             ${HEADERS} )

target_link_libraries( chaindata_plugin baseapp basechain fc )
target_include_directories( chaindata_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
