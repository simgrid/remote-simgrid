set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(wait_for main.cpp)

# Libraries to link
target_link_libraries(wait_for rsg)

target_link_libraries(wait_for "RsgService.thrift")
target_link_libraries(wait_for ${THRIFT_LIBRARIES})
target_link_libraries(wait_for ${SimGrid_LIBRARY})
target_link_libraries(wait_for ${ZMQ_LIBRARIES})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(conditionVariable-wait_for --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
