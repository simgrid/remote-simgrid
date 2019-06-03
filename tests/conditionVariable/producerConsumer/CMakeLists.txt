set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(producerConsumer main.cpp)

# Libraries to link
target_link_libraries(producerConsumer rsg)
target_link_libraries(producerConsumer "RsgService.thrift")
target_link_libraries(producerConsumer ${THRIFT_LIBRARIES})
target_link_libraries(producerConsumer ${ZMQ_LIBRARIES})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(conditionVariable-producer_consumer --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
