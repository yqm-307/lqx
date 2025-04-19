
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/scene)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../monitor/monitorclient/*.cc
    main.cc
)

add_executable(scene ${SRC_FILES})

target_link_libraries(scene
    bbt_core
    bbt_pollevent
    bbt_network
    bbt_coroutine
    bbt_rpc
    bbt_ecs
)

target_include_directories(scene
    PRIVATE
    ${CMAKE_SOURCE_DIR}/project
)