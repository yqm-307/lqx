
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/scenemanager)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cc
    main.cc
)

add_executable(scenemanager ${SRC_FILES})

target_link_libraries(scenemanager
    bbt_core
    bbt_pollevent
    bbt_network
    bbt_coroutine
    bbt_rpc
    boost_program_options
)

target_include_directories(scenemanager
    PRIVATE
    ${CMAKE_SOURCE_DIR}/project
)