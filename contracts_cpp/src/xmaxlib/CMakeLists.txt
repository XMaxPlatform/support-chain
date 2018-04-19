set(PROJECT_NAME xmaxlib)
set(TARGET_FILTER_NAME 6.ContractCpp)


file(GLOB HEADERS_HPP "${CONTRACTCPP_ROOT}/include/${PROJECT_NAME}/*.hpp")
file(GLOB HEADERS_H "${CONTRACTCPP_ROOT}/include/${PROJECT_NAME}/*.h")


add_library( ${PROJECT_NAME} ${HEADERS_HPP} ${HEADERS_H} ${PROJECT_NAME}.cpp)

target_include_directories(
        ${PROJECT_NAME} PUBLIC
        ${CONTRACTCPP_ROOT}/include/
)


#set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS_HPP} ${HEADERS_H}" FOLDER ${TARGET_FILTER_NAME})



install( FILES ${HEADERS_HPP} ${HEADERS_H} DESTINATION "contract/xmaxlib" )