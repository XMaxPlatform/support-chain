file(GLOB HEADERS "include/*.hpp")

## SORT .cpp by most likely to change / break compile
add_library( xmax_native_contract
        genesis_state.cpp
        native_contract_chain_init.cpp
        xmax_contract.cpp
		cash_contract.cpp
        xmax_voting.cpp
        ${HEADERS}
        )

target_link_libraries( xmax_native_contract basetypes xmaxchain fc )
target_include_directories( xmax_native_contract
        PUBLIC "include" "${CMAKE_CURRENT_BINARY_DIR}/include"
 )

if(USE_V8_VM)
target_include_directories(xmax_native_contract
PUBLIC "include"  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/include  ${CMAKE_CURRENT_SOURCE_DIR}/../chain/ )
endif()	

INSTALL( TARGETS
        xmax_native_contract

        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
INSTALL( FILES ${HEADERS} DESTINATION "include" )

set_target_properties( xmax_native_contract PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")