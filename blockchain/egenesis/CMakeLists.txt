
add_library( xmax_egenesis_none
        egenesis_none.cpp
        include/egenesis.hpp
   )

target_link_libraries( xmax_egenesis_none xmaxchain fc xmax_native_contract)
target_include_directories( xmax_egenesis_none
   PUBLIC "include" )

add_executable( embed_genesis
        embed_genesis.cpp
   )

set(SPECIFIC_LIBS "")

if(WIN32)
	list(APPEND SPECIFIC_LIBS 
		${Secp256k1_LIBRARY}/gmp.lib
		${Secp256k1_LIBRARY}/secp256k1.lib
		${OPENSSL_LIBRARY_DIR}/libcrypto.lib
		${OPENSSL_LIBRARY_DIR}/openssl.lib
		Userenv.lib
        )
        

        # if(NOT ("${OPENSSL_ROOT_DIR}" STREQUAL ""))
               
        #         add_custom_command(TARGET embed_genesis 
        #                          POST_BUILD
        #                          COMMAND  ${CMAKE_COMMAND} -E copy_if_different "${OPENSSL_ROOT_DIR}/bin/libcrypto-1_1-x64.dll"  "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/"      
        #                          )
               
        # endif()
endif()


target_link_libraries( embed_genesis xmaxchain xmax_egenesis_none fc xmax_native_contract ${SPECIFIC_LIBS})

set( embed_genesis_args
   -t "${CMAKE_CURRENT_SOURCE_DIR}/egenesis_brief.cpp.tmpl---${CMAKE_CURRENT_BINARY_DIR}/egenesis_brief.cpp"
   -t "${CMAKE_CURRENT_SOURCE_DIR}/egenesis_full.cpp.tmpl---${CMAKE_CURRENT_BINARY_DIR}/egenesis_full.cpp"
   )

MESSAGE( STATUS "egenesis: " ${XMAX_EGENESIS_JSON} )

if( XMAX_EGENESIS_JSON )
   list( APPEND embed_genesis_args --genesis-json "${XMAX_EGENESIS_JSON}" )
endif( XMAX_EGENESIS_JSON )

MESSAGE( STATUS "embed_genesis_args: " ${embed_genesis_args} )




add_custom_command(
   OUTPUT
      "${CMAKE_CURRENT_BINARY_DIR}/egenesis_brief.cpp"
      "${CMAKE_CURRENT_BINARY_DIR}/egenesis_full.cpp"
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND embed_genesis ${embed_genesis_args}
   DEPENDS
      "${XMAX_EGENESIS_JSON}"
      "egenesis_brief.cpp.tmpl"
      "egenesis_full.cpp.tmpl"
      embed_genesis )

add_library( xmax_egenesis_brief "${CMAKE_CURRENT_BINARY_DIR}/egenesis_brief.cpp" include/egenesis.hpp)
add_library( xmax_egenesis_full  "${CMAKE_CURRENT_BINARY_DIR}/egenesis_full.cpp" include/egenesis.hpp)

target_link_libraries( xmax_egenesis_brief xmaxchain xmax_native_contract fc )
target_link_libraries( xmax_egenesis_full xmaxchain xmax_native_contract fc )

target_include_directories( xmax_egenesis_brief
   PUBLIC "include" )
target_include_directories( xmax_egenesis_full
   PUBLIC "include" )

INSTALL( TARGETS
   embed_genesis xmax_egenesis_none xmax_egenesis_brief xmax_egenesis_full

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)



set_target_properties( embed_genesis PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")
set_target_properties( xmax_egenesis_none PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")
set_target_properties( xmax_egenesis_brief PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")
set_target_properties( xmax_egenesis_full PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")