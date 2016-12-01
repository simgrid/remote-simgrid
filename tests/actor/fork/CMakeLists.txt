set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(forkActor main.cpp)

# Libraries to link
target_link_libraries(forkActor rsg)

target_link_libraries(forkActor "RsgService.thrift")
target_link_libraries(forkActor ${THRIFT_LIBRARIES})
target_link_libraries(forkActor ${SimGrid_LIBRARY})
target_link_libraries(forkActor ${ZMQ_LIBRARIES})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_fork --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
