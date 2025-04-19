set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/gateway)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../monitor/monitorclient/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)

add_executable(gateway ${SRC_FILES})
target_include_directories(gateway
    PRIVATE
    ${CMAKE_SOURCE_DIR}/project
)
target_link_libraries(gateway
    bbt_core
    bbt_pollevent
    bbt_network
    bbt_coroutine
    bbt_rpc
)