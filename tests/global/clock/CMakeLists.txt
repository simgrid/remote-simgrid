set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(getClockMain main.cpp)

# Libraries to link
target_link_libraries(getClockMain rsg)

target_link_libraries(getClockMain "RsgService.thrift")
target_link_libraries(getClockMain ${THRIFT_LIBRARIES})
target_link_libraries(getClockMain ${SimGrid_LIBRARY})
target_link_libraries(getClockMain ${ZMQ_LIBRARIES})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(global_get_clock --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
