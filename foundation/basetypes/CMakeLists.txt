file(GLOB HEADERS "include/*.hpp")
file (GLOB SOURCE_FILES "*.cpp")



add_subdirectory( types_generator )





add_library( basetypes ${SOURCE_FILES} ${HEADERS} )
target_link_libraries( basetypes fc xmaxutilities )
target_include_directories( basetypes PUBLIC "include" "${CMAKE_CURRENT_SOURCE_DIR}" )
set_target_properties( basetypes PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "1.Foundation")





