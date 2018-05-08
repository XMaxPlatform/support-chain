file(GLOB HEADERS "include/*.hpp")
add_library( chainnet_plugin
			 connection_xmax.cpp
             chainnet_plugin.cpp
			 sync_main.cpp
             ${HEADERS} )

target_link_libraries( chainnet_plugin blockchain_plugin blockbuilder_plugin baseapp fc )
target_include_directories( chainnet_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   chainnet_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include" )
set_target_properties( chainnet_plugin PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "5.Plugins")