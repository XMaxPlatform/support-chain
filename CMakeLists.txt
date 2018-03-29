# Defines XMAX library target.
project( Xmax )
cmake_minimum_required( VERSION 3.11.0 )

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set( BLOCKCHAIN_NAME "XMax" )
set( CMAKE_CXX_STANDARD 14 )

SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )
IF( WIN32 )
  SET(BOOST_ROOT $ENV{BOOST_ROOT})
  set(Boost_USE_MULTITHREADED ON)
  set(BOOST_ALL_DYN_LINK OFF) # force dynamic linking for all libraries
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF(WIN32)
FIND_PACKAGE(Boost 1.64 REQUIRED COMPONENTS
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
    locale)

IF( WIN32 )

    message( STATUS "Configuring XMax on WIN32")

    #looks like this flag can have different default on some machines.
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

    # Probably cmake has a bug and vcxproj generated for executable in Debug conf. has disabled debug info
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

ELSE( WIN32 ) # Apple AND Linux

	message( STATUS "TODO" )

ENDIF( WIN32 )

add_subdirectory( foundation )



