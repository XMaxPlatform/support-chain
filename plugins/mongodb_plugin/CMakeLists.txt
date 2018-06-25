file(GLOB HEADERS "include/*.hpp")
add_library( mongodb_plugin
        mongodb_plugin.cpp
             ${HEADERS} )

if(MongoDB_FOUND)
    add_definitions(-DMONGO_DB=1)
endif()             


target_include_directories( mongodb_plugin 
                           PRIVATE ${MongoDB_INCLUDE_DIR}
                            PUBLIC 
                            "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/basetypes/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/basetypes"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/utilities/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../foundation/basechain/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../blockchain/native_contract/include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../../blockchain/chain/include"
                             )

# target_compile_definitions(mongodb_plugin
#                              PRIVATE ${LIBMONGOCXX_DEFINITIONS} ${LIBBSONCXX_DEFINITIONS}
#                              )                             


target_link_libraries( mongodb_plugin 
                        PUBLIC  baseapp fc xmaxchain
                        ${MongoDB_LIBRARIES})

set_target_properties( mongodb_plugin PROPERTIES
                       # PUBLIC_HEADER "${HEADERS}" 
                        FOLDER "5.Plugins")


install( TARGETS
   mongodb_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)


# install( FILES ${HEADERS} DESTINATION "include/xmax/mongodb_plugin" )