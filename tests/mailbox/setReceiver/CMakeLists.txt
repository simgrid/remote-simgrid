set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(mailbox-getReceiver sender.cpp)

# Libraries to link
target_link_libraries(mailbox-getReceiver rsg)

target_link_libraries(mailbox-getReceiver "RsgService.thrift")
target_link_libraries(mailbox-getReceiver ${THRIFT_LIBRARIES})
target_link_libraries(mailbox-getReceiver ${SimGrid_LIBRARY})


add_executable(mailbox-setReceiver receiver.cpp)

# Libraries to link
target_link_libraries(mailbox-setReceiver rsg)

target_link_libraries(mailbox-setReceiver "RsgService.thrift")
target_link_libraries(mailbox-setReceiver ${THRIFT_LIBRARIES})
target_link_libraries(mailbox-setReceiver ${SimGrid_LIBRARY})

configure_file(deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/deploy.xml")
ADD_TESH(mailbox-mailbox_setgetReceiver --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/client_test.tesh)
