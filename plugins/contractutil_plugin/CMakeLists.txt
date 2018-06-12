file(GLOB HEADERS "include/*.hpp")
add_library( contractutil_plugin
        contractutil_plugin.cpp
             ${HEADERS} )

target_link_libraries( contractutil_plugin baseapp fc basetypes xmaxutilities xmaxchain chainhttp_plugin xmax_native_contract WAST)
target_include_directories( contractutil_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( contractutil_plugin PUBLIC "../../blockchain/chain/" )
target_include_directories( contractutil_plugin PUBLIC "../../foundation/basetypes/" )
target_include_directories( contractutil_plugin PUBLIC "../../foundation/basetypes/include" )
set_target_properties( contractutil_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")