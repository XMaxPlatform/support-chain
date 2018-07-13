file(GLOB HEADERS "include/*.hpp" "include/objects/*.hpp")
file (GLOB SOURCE_FILES "*.cpp" "objects/*.cpp")

if(USE_V8_VM)
file (GLOB JSVM_BIND "jsvm_objbind/*.cpp" "jsvm_objbind/*.h" )
source_group("jsvm_bind"            FILES ${JSVM_BIND})
file (GLOB JSVM_UTIL "jsvm_util/*.cpp" "jsvm_util/*.h" )
source_group("jsvm_util"            FILES ${JSVM_UTIL})
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj") 
endif(MSVC)

if(USE_V8_VM)
add_library( xmaxchain ${SOURCE_FILES} ${HEADERS} ${JSVM_BIND} ${JSVM_UTIL})
else()
add_library( xmaxchain ${SOURCE_FILES} ${HEADERS}  )
endif()

target_link_libraries(
        xmaxchain fc basetypes basechain xmax_native_contract
        Runtime IR WASM
        ${Boost_LIBRARIES} ${PLATFORM_LIBRARIES} )
target_include_directories(
        xmaxchain PUBLIC "include"
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../virtualmachine/wasm-jit/Include
        ${CMAKE_SOURCE_DIR}/contracts_cpp/include
)

if(USE_V8_VM)
target_include_directories(xmaxchain  PUBLIC "include" ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/include )
endif()

set_target_properties( xmaxchain PROPERTIES PUBLIC_HEADER "${HEADERS}" FOLDER "4.BlockChain")
