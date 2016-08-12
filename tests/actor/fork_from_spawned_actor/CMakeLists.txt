set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(forkFromSpawnedActor main.cpp)

# Libraries to link
target_link_libraries(forkFromSpawnedActor rsg)

target_link_libraries(forkFromSpawnedActor "RsgService.thrift")
target_link_libraries(forkFromSpawnedActor ${THRIFT_LIBRARIES})
target_link_libraries(forkFromSpawnedActor ${SimGrid_LIBRARY})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_fork_spawned --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
