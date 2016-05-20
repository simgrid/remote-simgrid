

#cmake don't rebuild executables when you change tests:
#http://stackoverflow.com/questions/733475/cmake-ctest-make-test-doesnt-build-tests
#(and yes for cmake devs it's a feature, not a bug!)
#you have to do make;ctest when developping your tests!
set(TEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

## Add your new test directories here:
add_subdirectory(basic)
add_subdirectory(actor/basic)
add_subdirectory(comm/async_send_receive)
add_subdirectory(comm/async_send_receive_data_type)
add_subdirectory(mailbox/send_receive)
add_subdirectory(mailbox/send_receive_data_type)
add_subdirectory(mailbox/two_way_comm)
add_subdirectory(turnOffTurnOn)
