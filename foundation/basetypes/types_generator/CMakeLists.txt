file(GLOB HEADERS "include/*.hpp")
file (GLOB SOURCE_FILES "*.cpp")


set(SPECIFIC_LIBS "")

if(WIN32)
	list(APPEND SPECIFIC_LIBS 
		Userenv
	)
endif()


add_executable( types_generator type_generator.cpp type_parser.cpp)


target_include_directories( types_generator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../include" )
target_link_libraries( types_generator basetypes fc xmaxutilities ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} ${SPECIFIC_LIBS})
set_target_properties( types_generator PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "1.Foundation")


set(GEN_TYPES_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../include/gentypes.hpp)

add_custom_command(OUTPUT "${GEN_TYPES_HEADER}"
			COMMAND $<TARGET_FILE:types_generator> ${CMAKE_CURRENT_SOURCE_DIR}/types.xmax ${GEN_TYPES_HEADER}
			DEPENDS types.xmax types_generator
			)
			
add_custom_target(generate_types_header ALL DEPENDS ${GEN_TYPES_HEADER} SOURCES types.xmax) 			

set_target_properties(generate_types_header PROPERTIES FOLDER "Tools")