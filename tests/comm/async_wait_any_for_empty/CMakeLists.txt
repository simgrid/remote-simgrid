set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(async_wait_any_for_empty receiver.cpp)

# Libraries to link
target_link_libraries(async_wait_any_for_empty rsg)

target_link_libraries(async_wait_any_for_empty "RsgService.thrift")
target_link_libraries(async_wait_any_for_empty ${THRIFT_LIBRARIES})
target_link_libraries(async_wait_any_for_empty ${SimGrid_LIBRARY})
target_link_libraries(async_wait_any_for_empty ${ZMQ_LIBRARIES})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(async_wait_any_for_empty --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
