file(GLOB HEADERS "include/*.hpp")
add_library( wallet_plugin
        wallet_plugin.cpp
             ${HEADERS} )

target_link_libraries( wallet_plugin 
        baseapp basetypes basechain fc chainhttp_plugin xmaxutilities )

target_include_directories( wallet_plugin 
                                PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                                PRIVATE "../../blockchain/chain/include")


set_target_properties( wallet_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")