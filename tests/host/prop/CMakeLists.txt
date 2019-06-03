set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(get_prop main.cpp)

# Libraries to link
target_link_libraries(get_prop rsg)

target_link_libraries(get_prop "RsgService.thrift")
target_link_libraries(get_prop ${THRIFT_LIBRARIES})
target_link_libraries(get_prop ${ZMQ_LIBRARIES})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(host_prop --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
