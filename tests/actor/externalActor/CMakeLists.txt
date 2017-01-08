set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(actorExternalMain main.cpp)

# Libraries to link
target_link_libraries(actorExternalMain rsg)

target_link_libraries(actorExternalMain "RsgService.thrift")
target_link_libraries(actorExternalMain ${THRIFT_LIBRARIES})
target_link_libraries(actorExternalMain ${SimGrid_LIBRARY})
target_link_libraries(actorExternalMain ${ZMQ_LIBRARIES})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_external_basic --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
