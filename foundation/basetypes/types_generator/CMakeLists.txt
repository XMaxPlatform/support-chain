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


if(WIN32)

	#set(TG_EXE_NAME_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug/types_generator.exe)
	#set(TG_EXE_NAME_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release/types_generator.exe)	
	#set(TG_EXE_NAME ${CMAKE_CURRENT_BINARY_DIR}/types_generator.exe)

else()

	set(TG_EXE_NAME ${CMAKE_CURRENT_BINARY_DIR}/types_generator)
	add_custom_command(TARGET types_generator
			POST_BUILD
			COMMAND ${TG_EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/types.xmax ${CMAKE_CURRENT_SOURCE_DIR}/../include/gentypes.hpp
			DEPENDS types.xmax types_generator
			)
endif()

