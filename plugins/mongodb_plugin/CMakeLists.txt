file(GLOB HEADERS "include/*.hpp")
add_library( mongodb_plugin
        mongodb_plugin.cpp
             ${HEADERS} )


if( WIN32 )

        if("${MONGO_DB_C_ROOT}" STREQUAL "")
                set(MONGO_DB_C_ROOT "c:/mongo-c-driver")
        endif()

        if("${MONGO_DB_CXX_ROOT}" STREQUAL "")
                set(MONGO_DB_CXX_ROOT "c:/mongo-cxx-driver")            
        endif()


endif(WIN32)     


find_package(libbson-1.0
HINTS "${MONGO_DB_C_ROOT}")

find_package(libmongoc-1.0 1.8
HINTS "${MONGO_DB_C_ROOT}")


                

if (libmongoc-1.0_FOUND)

    # XMX has no direct dependencies on libmongoc but its shared libraries
    # will need to be present at runtime for the C++ libraries we use:
    # libbsoncxx & libmongocxx (both from github.com/mongodb/mongo-cxx-driver)

    # The *.cmake package files provided by mongo-cxx-driver don't give us the
    # absolute path to the libraries, which is needed whenever they are not
    # installed in system-known locations.  CMake requires the absolute paths
    # in target_link_libraries() since we are builiding an archive and the
    # link step for all executables using this archive must include the
    # mongo-cxx-driver libraries libmongocxx and libbsoncxx.
    
    set(XMAX_LIBBSONCXX "")
    find_package(libbsoncxx REQUIRED HINTS "${MONGO_DB_CXX_ROOT}")

    message(STATUS "Found bsoncxx headers: ${LIBBSONCXX_INCLUDE_DIRS}")
    
    if((LIBMONGOCXX_VERSION_MAJOR LESS 3) OR ((LIBMONGOCXX_VERSION_MAJOR EQUAL 3) AND (LIBMONGOCXX_VERSION_MINOR LESS 2)))
        find_library(XMAX_LIBBSONCXX ${LIBBSONCXX_LIBRARIES}
      PATHS ${LIBBSONCXX_LIBRARY_DIRS} NO_DEFAULT_PATH)
    else()
        set(XMAX_LIBBSONCXX ${LIBBSONCXX_LIBRARIES})
    endif()
    message(STATUS "Found bsoncxx library: ${XMAX_LIBBSONCXX}")

    
    set(XMAX_LIBMONGOCXX "")  
    find_package(libmongocxx REQUIRED HINTS "${MONGO_DB_CXX_ROOT}")
    message(STATUS "Found mongocxx headers: ${LIBMONGOCXX_INCLUDE_DIRS}")

     # mongo-cxx-driver 3.2 release altered LIBBSONCXX_LIBRARIES semantics. Instead of library names,
      #  it now hold library paths.
    if((LIBMONGOCXX_VERSION_MAJOR LESS 3) OR ((LIBMONGOCXX_VERSION_MAJOR EQUAL 3) AND (LIBMONGOCXX_VERSION_MINOR LESS 2)))
        find_library(XMAX_LIBMONGOCXX ${LIBMONGOCXX_LIBRARIES}
          PATHS ${LIBMONGOCXX_LIBRARY_DIRS} NO_DEFAULT_PATH)
      else()
        set(XMAX_LIBMONGOCXX ${LIBMONGOCXX_LIBRARIES})
    endif()
  
    message(STATUS "Found mongocxx library: ${XMAX_LIBMONGOCXX}")

    add_definitions(-DMONGODB=1)


else()
    message("Could NOT find MongoDB. mongodb_plugin with MongoDB support will not be included.")

endif()


target_include_directories( mongodb_plugin 
                           PRIVATE ${LIBMONGOCXX_INCLUDE_DIRS} ${LIBBSONCXX_INCLUDE_DIRS}
                            PUBLIC 
                            "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/basetypes/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/basetypes"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/utilities/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/basechain/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../blockchain/native_contract/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../blockchain/chain/include"
                             )

target_compile_definitions(mongodb_plugin
                             PRIVATE ${LIBMONGOCXX_DEFINITIONS} ${LIBBSONCXX_DEFINITIONS}
                             )                             


target_link_libraries( mongodb_plugin 
                        PUBLIC  baseapp fc xmaxchain
                        ${XMAX_LIBMONGOCXX} ${XMAX_LIBBSONCXX})

set_target_properties( mongodb_plugin PROPERTIES
                       # PUBLIC_HEADER "${HEADERS}" 
                        FOLDER "5.Plugins")


if(WIN32 AND libmongoc-1.0_FOUND)

    string(REGEX REPLACE "/" "\\\\" MONGO_C_DLLS ${MONGO_DB_C_ROOT}/bin/*.dll)
    string(REGEX REPLACE "/" "\\\\" MONGO_CXX_DLLS ${MONGO_DB_CXX_ROOT}/bin/*.dll)
    string(REGEX REPLACE "/" "\\\\" RUNTIME_OUTPUT_DIR_WIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    add_custom_command(TARGET mongodb_plugin
                        POST_BUILD
                        COMMAND  copy "/y" "${MONGO_C_DLLS}" "${RUNTIME_OUTPUT_DIR_WIN}"
                        COMMAND  copy "/y" "${MONGO_CXX_DLLS}" "${RUNTIME_OUTPUT_DIR_WIN}"                        
                        )
endif()                        

install( TARGETS
   mongodb_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)


if(WIN32 AND  NOT ("${MONGO_DB_CXX_ROOT}" STREQUAL ""))
#    message("Found db cxx root and install.")       
                install(DIRECTORY ${MONGO_DB_CXX_ROOT}/bin/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        FILES_MATCHING PATTERN "*.dll") 
        
        install(DIRECTORY ${MONGO_DB_CXX_ROOT}/bin/ DESTINATION ${RUNTIME_BUILD_PATH}
        FILES_MATCHING PATTERN "*.dll") 

else()
    message("Cannot find MONGO_DB_CXX_ROOT root and install.")        
endif()

if(WIN32 AND  NOT ("${MONGO_DB_C_ROOT}" STREQUAL ""))
#    message("Found db cxx root and install.")       
                install(DIRECTORY ${MONGO_DB_C_ROOT}/bin/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        FILES_MATCHING PATTERN "*.dll")  

             install(DIRECTORY ${MONGO_DB_C_ROOT}/bin/ DESTINATION ${RUNTIME_BUILD_PATH}
        FILES_MATCHING PATTERN "*.dll") 
else()
    message("Cannot find MONGO_DB_C_ROOT and install.")        
endif()


# install( FILES ${HEADERS} DESTINATION "include/xmax/mongodb_plugin" )