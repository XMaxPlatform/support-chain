# Defines XMAX library target.
project( Xmax )
IF( WIN32 )
    cmake_minimum_required( VERSION 3.11.0 )
ELSE( WIN32)
    cmake_minimum_required( VERSION 2.8.12 )
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
            message(FATAL_ERROR "GCC version must be at least 4.8!")
        endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
            message(FATAL_ERROR "Clang version must be at least 3.3!")
        endif()
    endif()
ENDIF(WIN32)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/fc/CMakeModules")
set(XMAX_EGENESIS_JSON "${CMAKE_CURRENT_SOURCE_DIR}/genesis.json" )


set(XMAX_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set( BLOCKCHAIN_NAME "XMax" )
set( CMAKE_CXX_STANDARD 14 )

if(WIN32)
  include(windows_config)


endif()


SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )
IF( WIN32 )

  message( STATUS "BOOST_ROOT:${BOOST_ROOT}")

  set(Boost_USE_MULTITHREADED ON)
  set(BOOST_ALL_DYN_LINK OFF) # force dynamic linking for all libraries
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF(WIN32)
FIND_PACKAGE(Boost 1.66 REQUIRED COMPONENTS
    thread
    date_time
    system
    filesystem
    program_options
    signals
    serialization
    chrono	
    unit_test_framework
    context
    locale

	regex
	random
	
	)

find_package(MongoDB)

IF( WIN32 )

    message( STATUS "Configuring XMax on WIN32")

    
    set(RUNTIME_BUILD_PATH  "${PROJECT_BINARY_DIR}/bin")

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")


    #looks like this flag can have different default on some machines.
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

    # Probably cmake has a bug and vcxproj generated for executable in Debug conf. has disabled debug info
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

ELSE( WIN32 ) # Apple AND Linux

    find_library(READLINE_LIBRARIES NAMES readline)
    find_path(READLINE_INCLUDE_DIR readline/readline.h)
    #if(NOT READLINE_INCLUDE_DIR OR NOT READLINE_LIBRARIES)
    #  MESSAGE(FATAL_ERROR "Could not find lib readline.")
    #endif()

    if( APPLE )
        # Apple Specific Options Here
        message( STATUS "Configuring XMax on OS X" )
        #set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -Wall -Wno-invalid-partial-specialization -Wno-deprecated-declarations" )
        set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -Wall -Wno-deprecated-declarations" )
    else( APPLE )
        # Linux Specific Options Here
        message( STATUS "Configuring XMax on Linux" )
        set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall" )
        set( rt_library rt )
        set( pthread_library pthread)
        if ( NOT DEFINED crypto_library )
            # I'm not sure why this is here, I guess someone has openssl and can't detect it with find_package()?
            # if you have a normal install, you can define crypto_library to the empty string to avoid a build error
            set( crypto_library crypto)
        endif ()
        if ( FULL_STATIC_BUILD )
            set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
        endif ( FULL_STATIC_BUILD )

        if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
            if( CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.0.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.0.0 )
                set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-partial-specialization" )
            endif()
        endif()
    endif( APPLE )

    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-memcmp" )
    endif()

    if( "${CMAKE_GENERATOR}" STREQUAL "Ninja" )
        if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics" )
        endif()
    endif()

    # based on http://www.delorie.com/gnu/docs/gdb/gdb_70.html
    # uncomment this line to tell GDB about macros (slows compile times)
    # set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -g3" )

ENDIF( WIN32 )


include(wasm)

message(STATUS "---------------------------- wasm info ----------------------------")
message(STATUS "LLVM_DIR: ${LLVM_DIR}")
message(STATUS "BINARYEN_BIN: ${BINARYEN_BIN}")
message(STATUS "Using WASM clang: " ${WASM_CLANG})
message(STATUS "Using WASM llc: " ${WASM_LLC})
message(STATUS "Using WASM llvm-link: " ${WASM_LLVM_LINK})
message(STATUS "---------------------------- wasm info end ------------------------")
if(NOT WASM_TOOLCHAIN)
    message(STATUS "--------------------------------------------------------------------")
    message(STATUS "No WASM compiler could be found ... (skipping building of contracts)")
    message(STATUS "--------------------------------------------------------------------")
endif()

include(installer)

add_subdirectory( foundation )
add_subdirectory( libraries )
add_subdirectory( virtualmachine )
add_subdirectory( blockchain )
add_subdirectory( plugins )
add_subdirectory( launch )
add_subdirectory( contracts_cpp )
add_subdirectory( tests )

if(WIN32 AND NOT ("${OPENSSL_ROOT_DIR}" STREQUAL ""))    

    install(DIRECTORY ${OPENSSL_ROOT_DIR}/bin/ DESTINATION bin
        FILES_MATCHING PATTERN "libcrypto-1_1-x64.dll") 
        
    install(DIRECTORY ${OPENSSL_ROOT_DIR}/bin/ DESTINATION ${RUNTIME_BUILD_PATH}
        FILES_MATCHING PATTERN "libcrypto-1_1-x64.dll")        
endif()

