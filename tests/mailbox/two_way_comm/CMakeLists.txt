set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(node1 node1.cpp)
target_link_libraries(node1 thriftClient)
target_link_libraries(node1 "RsgService.thrift")
target_link_libraries(node1 ${THRIFT_LIBRARIES})
target_link_libraries(node1 ${SimGrid_LIBRARY})


add_executable(node2 node2.cpp)
target_link_libraries(node2 thriftClient)
target_link_libraries(node2 "RsgService.thrift")
target_link_libraries(node2 ${THRIFT_LIBRARIES})
target_link_libraries(node2 ${SimGrid_LIBRARY})

add_executable(node3 node3.cpp)
target_link_libraries(node3 thriftClient)
target_link_libraries(node3 "RsgService.thrift")
target_link_libraries(node3 ${THRIFT_LIBRARIES})
target_link_libraries(node3 ${SimGrid_LIBRARY})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(two_way_send_receive --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
