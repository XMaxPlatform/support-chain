
file(GLOB Sources "*.cpp")
file(GLOB Headers "include/*.h")

add_executable(client_tools ${Headers} ${Sources})

target_include_directories(client_tools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(client_tools 
                    PRIVATE fc ${Boost_LIBRARIES} 
                    PRIVATE basetypes xmaxutilities)

set_target_properties( client_tools PROPERTIES FOLDER "Tools")
