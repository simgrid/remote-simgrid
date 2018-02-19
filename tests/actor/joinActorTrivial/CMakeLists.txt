set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(joinActorTrivial main.cpp)

# Libraries to link
target_link_libraries(joinActorTrivial rsg)

target_link_libraries(joinActorTrivial "RsgService.thrift")
target_link_libraries(joinActorTrivial ${THRIFT_LIBRARIES})
target_link_libraries(joinActorTrivial ${SimGrid_LIBRARY})
target_link_libraries(joinActorTrivial ${ZMQ_LIBRARIES})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_join_trivial --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
