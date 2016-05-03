set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(client_test_server main_client.cpp)

# Libraries to link
target_link_libraries(client_test_server thriftClient)

target_link_libraries(client_test_server "RsgService.thrift")
target_link_libraries(client_test_server ${THRIFT_LIBRARIES})
target_link_libraries(client_test_server ${SimGrid_LIBRARY})


add_executable(actor_test_server main_server_test.cpp)

# Libraries to link
target_link_libraries(actor_test_server thriftClient)

target_link_libraries(actor_test_server "RsgService.thrift")
target_link_libraries(actor_test_server ${THRIFT_LIBRARIES})
target_link_libraries(actor_test_server ${SimGrid_LIBRARY})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_client_server --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
