file(GLOB HEADERS "include/*.hpp")
file (GLOB SOURCE_FILES "*.cpp")




add_executable( types_generator type_generator.cpp type_parser.cpp)


target_include_directories( types_generator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../include" )
target_link_libraries( types_generator basetypes fc xmaxutilities ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS}  )
set_target_properties( types_generator PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "1.Foundation")

add_custom_command(TARGET types_generator
        POST_BUILD
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/types_generator ${CMAKE_CURRENT_SOURCE_DIR}/types.xmax ${CMAKE_CURRENT_SOURCE_DIR}/../include/gentypes.hpp
        DEPENDS types.xmax types_generator
        )