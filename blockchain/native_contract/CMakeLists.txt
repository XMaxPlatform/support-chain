file(GLOB HEADERS "include/*.hpp")

## SORT .cpp by most likely to change / break compile
add_library( xmax_native_contract
        genesis_state.cpp
        native_contract_chain_init.cpp
        ${HEADERS}
        )

target_link_libraries( xmax_native_contract xmaxchain fc )
target_include_directories( xmax_native_contract
        PUBLIC "include" "${CMAKE_CURRENT_BINARY_DIR}/include" )

INSTALL( TARGETS
        xmax_native_contract

        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
INSTALL( FILES ${HEADERS} DESTINATION "include" )

set_target_properties( xmax_native_contract PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")