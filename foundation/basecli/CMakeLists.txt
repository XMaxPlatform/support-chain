
set(lib_name basecli)
project( ${lib_name} )
cmake_minimum_required( VERSION 2.8.12 )

list(APPEND CMAKE_MODULE_PATH "CMakeModules")

file(GLOB HEADERS "include/*.hpp")
file(GLOB SOURCES "*.cpp")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")


add_library( ${lib_name}
         ${HEADERS}
         ${SOURCES}
           )

target_link_libraries( ${lib_name})

target_include_directories( ${lib_name}
                            PUBLIC "include" )

set_target_properties( ${lib_name} PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "1.Foundation")

set(CPACK_PACKAGING_INSTALL_PREFIX ..)