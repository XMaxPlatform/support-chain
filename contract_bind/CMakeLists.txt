file(GLOB SRC "*.h" "*.cpp")
add_library( jsvmbind
             ${SRC} )

target_link_libraries( jsvmbind baseapp fc basetypes xmaxutilities xmaxchain chainhttp_plugin xmax_native_contract WAST)
target_include_directories( jsvmbind PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( jsvmbind PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
target_include_directories( jsvmbind PUBLIC "../blockchain/chain/" )
target_include_directories( jsvmbind PUBLIC "../foundation/basetypes/" )
target_include_directories( jsvmbind PUBLIC "../foundation/basetypes/include" )
set_target_properties( jsvmbind PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "7.ContractBind")