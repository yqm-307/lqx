
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/monitor)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

add_executable(monitor ${SRC_FILES})

target_link_libraries(monitor
    bbt_core
    bbt_pollevent
    bbt_network
    bbt_coroutine
    boost_program_options
)

target_include_directories(monitor
    PRIVATE
    ${CMAKE_SOURCE_DIR}/project
)