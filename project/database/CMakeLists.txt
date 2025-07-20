set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/database)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../monitor/monitorclient/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/dbhelper/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)


add_executable(database ${SRC_FILES})

target_include_directories(database
    PRIVATE
    ${CMAKE_SOURCE_DIR}/project
)
target_link_libraries(database
    bbt_core
    bbt_network
    bbt_coroutine
    bbt_rpc
    
    mysqlpp
)



add_subdirectory(test)