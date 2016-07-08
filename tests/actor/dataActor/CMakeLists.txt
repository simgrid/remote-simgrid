set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(dataMain main.cpp)

# Libraries to link
target_link_libraries(dataMain rsg)

target_link_libraries(dataMain "RsgService.thrift")
target_link_libraries(dataMain ${THRIFT_LIBRARIES})
target_link_libraries(dataMain ${SimGrid_LIBRARY})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_data_params --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
