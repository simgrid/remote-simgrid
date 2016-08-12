set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(getPpidMain main.cpp)

# Libraries to link
target_link_libraries(getPpidMain rsg)

target_link_libraries(getPpidMain "RsgService.thrift")
target_link_libraries(getPpidMain ${THRIFT_LIBRARIES})
target_link_libraries(getPpidMain ${SimGrid_LIBRARY})


configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(actor_getPPID --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
