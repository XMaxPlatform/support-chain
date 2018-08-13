set( lib_name xmaxcl )

file(GLOB HEADERS "include/*.hpp")
file(GLOB SOURCES "*.cpp")


add_executable( ${lib_name} ${HEADERS} ${SOURCES} )

if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries( ${lib_name}
        PRIVATE basecli	basetypes
        PRIVATE xmaxchain ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )

set_target_properties( ${lib_name} PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "8.Launch")

install( TARGETS 
${lib_name} 

RUNTIME DESTINATION bin 
LIBRARY DESTINATION lib 
ARCHIVE DESTINATION lib 
)