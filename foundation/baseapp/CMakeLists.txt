project( BaseApp )
cmake_minimum_required( VERSION 2.8.12 )

list(APPEND CMAKE_MODULE_PATH "CMakeModules")

file(GLOB HEADERS "include/*.hpp")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(BOOST_COMPONENTS)
list(APPEND BOOST_COMPONENTS thread
                             date_time
                             filesystem
                             system
                             chrono
                             program_options
                             unit_test_framework
                             locale)

find_package(Boost 1.64 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
set( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )

if( APPLE )

  message( STATUS "Configuring App on OS X" )
  set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -stdlib=libc++ -Wall -Wno-conversion -Wno-deprecated-declarations" )
else( APPLE )
  IF( WIN32 )

  message( STATUS "Configuring BaseChain on Windows" )
  set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -Wall -EHsc" )
  else( WIN32 )
  # Linux Specific Options Here
  message( STATUS "Configuring App on Linux" )
  set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -Wall" )
  set( rt_library rt )
  set( pthread_library pthread)
  if ( FULL_STATIC_BUILD )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
  endif ( FULL_STATIC_BUILD )
  endif( WIN32 )
endif( APPLE )


if(ENABLE_COVERAGE_TESTING)
    SET(CMAKE_CXX_FLAGS "--coverage ${CMAKE_CXX_FLAGS}")
endif()

add_library( baseapp
        application.cpp
             ${HEADERS}
           )

target_link_libraries( baseapp ${Boost_LIBRARIES})

target_include_directories( baseapp
                            PUBLIC "include" ${Boost_INCLUDE_DIR})

set_target_properties( baseapp PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "1.Foundation")

set(CPACK_PACKAGING_INSTALL_PREFIX ..)