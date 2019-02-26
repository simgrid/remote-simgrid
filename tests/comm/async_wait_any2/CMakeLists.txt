set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(async_wait_any2 sender.cpp)

# Libraries to link
target_link_libraries(async_wait_any2 rsg)

target_link_libraries(async_wait_any2 "RsgService.thrift")
target_link_libraries(async_wait_any2 ${THRIFT_LIBRARIES})
target_link_libraries(async_wait_any2 ${SimGrid_LIBRARY})
target_link_libraries(async_wait_any2 ${ZMQ_LIBRARIES})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(async_wait_any2 --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
