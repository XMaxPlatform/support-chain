add_subdirectory(chain)
add_subdirectory(egenesis)
add_subdirectory(native_contract)


configure_file( contract_compiler.in contract_compiler @ONLY)
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/contract_compiler DESTINATION bin/ PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE WORLD_READ GROUP_READ OWNER_READ )

