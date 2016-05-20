set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(main_client main_client.cpp)

# Libraries to link
target_link_libraries(main_client thriftClient)

target_link_libraries(main_client "RsgService.thrift")
target_link_libraries(main_client ${THRIFT_LIBRARIES})
target_link_libraries(main_client ${SimGrid_LIBRARY})


add_executable(main_server_test main_server_test.cpp)

# Libraries to link
target_link_libraries(main_server_test thriftClient)

target_link_libraries(main_server_test "RsgService.thrift")
target_link_libraries(main_server_test ${THRIFT_LIBRARIES})
target_link_libraries(main_server_test ${SimGrid_LIBRARY})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(basic_client_server --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
