set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(tontoff_client simple_client.cpp)

# Libraries to link
target_link_libraries(tontoff_client rsg)

target_link_libraries(tontoff_client "RsgService.thrift")
target_link_libraries(tontoff_client ${THRIFT_LIBRARIES})
target_link_libraries(tontoff_client ${SimGrid_LIBRARY})


add_executable(tontoff_server main.cpp)

# Libraries to link
target_link_libraries(tontoff_server rsg)

target_link_libraries(tontoff_server "RsgService.thrift")
target_link_libraries(tontoff_server ${THRIFT_LIBRARIES})
target_link_libraries(tontoff_server ${SimGrid_LIBRARY})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(turnon_turnoff --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
