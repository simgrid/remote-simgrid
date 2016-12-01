set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(mutexBasic main.cpp)

# Libraries to link
target_link_libraries(mutexBasic rsg)

target_link_libraries(mutexBasic "RsgService.thrift")
target_link_libraries(mutexBasic ${THRIFT_LIBRARIES})
target_link_libraries(mutexBasic ${SimGrid_LIBRARY})
target_link_libraries(mutexBasic ${ZMQ_LIBRARIES})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(mutex_basic --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/mutexBasic.tesh)
