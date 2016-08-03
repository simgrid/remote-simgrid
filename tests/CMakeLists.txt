

#cmake don't rebuild executables when you change tests:
#http://stackoverflow.com/questions/733475/cmake-ctest-make-test-doesnt-build-tests
#(and yes for cmake devs it's a feature, not a bug!)
#you have to do make;ctest when developping your tests!
set(TEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

## Add your new test directories here:
add_subdirectory(basic)
add_subdirectory(actor/basic)
add_subdirectory(actor/externalActor)
add_subdirectory(actor/fork)
add_subdirectory(actor/spawnActor)
add_subdirectory(actor/joinActor)
add_subdirectory(actor/dataActor)
add_subdirectory(actor/killActor)
add_subdirectory(comm/async_send_receive)
add_subdirectory(comm/async_send_receive_data_type)
add_subdirectory(comm/async_with_test)
add_subdirectory(comm/async_wait_any)
add_subdirectory(comm/sync_async_combined)
add_subdirectory(conditionVariable/producerConsumer)
add_subdirectory(conditionVariable/wait_for)
add_subdirectory(conditionVariable/notify)
add_subdirectory(global/clock)
add_subdirectory(host/prop)
add_subdirectory(mailbox/empty)
add_subdirectory(mailbox/send_receive)
add_subdirectory(mailbox/send_receive_data_type)
add_subdirectory(mailbox/two_way_comm)
add_subdirectory(mailbox/setReceiver)
add_subdirectory(mutex/basic)
add_subdirectory(turnOffTurnOn)
