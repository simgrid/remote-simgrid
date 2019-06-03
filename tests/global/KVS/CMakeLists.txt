set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(kvs main.cpp)

# Libraries to link
target_link_libraries(kvs rsg)
target_link_libraries(kvs "RsgService.thrift")
target_link_libraries(kvs ${THRIFT_LIBRARIES})
target_link_libraries(kvs ${ZMQ_LIBRARIES})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(global_KVS --setenv RSG_KVS_DATA=${TEST_RESOURCES_DIR}/data.kvs --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
