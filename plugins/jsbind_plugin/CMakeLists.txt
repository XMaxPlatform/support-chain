file(GLOB SRC "*.h" "*.cpp")
add_library( jsbind_plugin
             ${SRC} )

target_link_libraries( jsbind_plugin baseapp fc basetypes xmaxutilities xmaxchain chainhttp_plugin xmax_native_contract WAST)
target_include_directories( jsbind_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( jsbind_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
target_include_directories( jsbind_plugin PUBLIC "../../blockchain/chain/" )
target_include_directories( jsbind_plugin PUBLIC "../../foundation/basetypes/" )
target_include_directories( jsbind_plugin PUBLIC "../../foundation/basetypes/include" )
set_target_properties( jsbind_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")