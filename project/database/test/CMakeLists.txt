set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/database/test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

add_executable(test_database test_database.cc)
target_link_libraries(test_database
    mysqlpp
    # boost_program_options
    boost_unit_test_framework
    boost_test_exec_monitor
)
add_test(NAME test_database COMMAND test_database)