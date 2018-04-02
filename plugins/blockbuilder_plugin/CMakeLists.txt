file(GLOB HEADERS "include/*.hpp")
add_library( blockbuilder_plugin
        blockbuilder_plugin.cpp
             ${HEADERS} )

target_link_libraries( blockbuilder_plugin xmaxchain baseapp chaindata_plugin blockchain_plugin)
target_include_directories( blockbuilder_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
