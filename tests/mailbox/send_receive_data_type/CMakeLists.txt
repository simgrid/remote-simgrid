set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(send_simple_data_type sender.cpp)

# Libraries to link
target_link_libraries(send_simple_data_type thriftClient)

target_link_libraries(send_simple_data_type "RsgService.thrift")
target_link_libraries(send_simple_data_type ${THRIFT_LIBRARIES})
target_link_libraries(send_simple_data_type ${SimGrid_LIBRARY})


add_executable(receive_simple_data_type receiver.cpp)

# Libraries to link
target_link_libraries(receive_simple_data_type thriftClient)

target_link_libraries(receive_simple_data_type "RsgService.thrift")
target_link_libraries(receive_simple_data_type ${THRIFT_LIBRARIES})
target_link_libraries(receive_simple_data_type ${SimGrid_LIBRARY})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(send_receive_data_type --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
