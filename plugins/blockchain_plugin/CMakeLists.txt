file(GLOB HEADERS "include/*.hpp")
add_library( blockchain_plugin
        blockchain_plugin.cpp
             ${HEADERS} )

target_link_libraries( blockchain_plugin basetypes xmaxchain baseapp chaindata_plugin chainhttp_plugin xmax_native_contract)
target_include_directories( blockchain_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

set_target_properties( blockchain_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")