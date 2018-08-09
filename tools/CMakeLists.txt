

if(WIN32)

add_custom_target(copy_dependent_dlls)
add_custom_command(TARGET copy_dependent_dlls POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${V8_DEPENDCIES_DLLS}
       "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")       

set_target_properties(copy_dependent_dlls PROPERTIES FOLDER "Tools")       


endif(WIN32)




add_subdirectory(client_tools)