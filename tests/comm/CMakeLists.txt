set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(comm_node_O main_client.cpp)

# Libraries to link
target_link_libraries(comm_node_O thriftClient)

target_link_libraries(comm_node_O "RsgService.thrift")
target_link_libraries(comm_node_O ${THRIFT_LIBRARIES})
target_link_libraries(comm_node_O ${SimGrid_LIBRARY})


add_executable(comm_node_1 main_server_test.cpp)

# Libraries to link
target_link_libraries(comm_node_1 thriftClient)

target_link_libraries(comm_node_1 "RsgService.thrift")
target_link_libraries(comm_node_1 ${THRIFT_LIBRARIES})
target_link_libraries(comm_node_1 ${SimGrid_LIBRARY})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(comm_node --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
