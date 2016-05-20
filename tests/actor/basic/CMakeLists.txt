set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(actorMain main.cpp)

# Libraries to link
target_link_libraries(actorMain thriftClient)

target_link_libraries(actorMain "RsgService.thrift")
target_link_libraries(actorMain ${THRIFT_LIBRARIES})
target_link_libraries(actorMain ${SimGrid_LIBRARY})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_basic --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
