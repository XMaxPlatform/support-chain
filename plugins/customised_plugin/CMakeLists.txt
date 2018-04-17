file(GLOB HEADERS "include/*.hpp")
add_library( customised_plugin
        customised_plugin.cpp
             ${HEADERS} )

target_link_libraries( customised_plugin baseapp fc basetypes xmaxutilities xmaxchain chaindata_plugin chainhttp_plugin xmax_native_contract)
target_include_directories( customised_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( customised_plugin PUBLIC "../../blockchain/chain/" )
target_include_directories( customised_plugin PUBLIC "../../foundation/basetypes/" )
target_include_directories( customised_plugin PUBLIC "../../foundation/basetypes/include" )
set_target_properties( customised_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")