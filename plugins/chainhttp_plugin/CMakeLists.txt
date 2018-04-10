file(GLOB HEADERS "include/*.hpp")
add_library( chainhttp_plugin
        chainhttp_plugin.cpp
             ${HEADERS} )

target_link_libraries( chainhttp_plugin baseapp fc )
target_include_directories( chainhttp_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

set_target_properties( chainhttp_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")