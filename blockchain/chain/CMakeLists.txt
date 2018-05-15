file(GLOB HEADERS "include/*.hpp" "include/objects/*.hpp")
file (GLOB SOURCE_FILES "*.cpp" "objects/*.cpp")

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj") 
endif(MSVC)

add_library( xmaxchain ${SOURCE_FILES} ${HEADERS} )
target_link_libraries(
        xmaxchain fc basetypes basechain xmax_native_contract
        Runtime IR WASM
        ${Boost_LIBRARIES} ${PLATFORM_LIBRARIES} )
target_include_directories(
        xmaxchain PUBLIC "include"
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../virtualmachine/wasm-jit/Include
)
set_target_properties( xmaxchain PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")
