
# Where to find the platform files and such
set(TEST_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

# The binaries needed for each test
set(basic_exec node1 node2)

# Let's build these examples
add_custom_target(tests COMMENT "Recompiling the tests")

foreach(example basic)
    foreach(exec ${${example}_exec})
        add_executable       (${example}_${exec} EXCLUDE_FROM_ALL ${example}/${example}_${exec}.cpp)
        set_target_properties(${example}_${exec} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${example})
	add_dependencies     (tests ${example}_${exec})
	
	target_link_libraries(${example}_${exec} rsg)
	target_link_libraries(${example}_${exec} "RsgService.thrift")
	target_link_libraries(${example}_${exec} ${THRIFT_LIBRARIES})
	target_link_libraries(${example}_${exec} ${ZMQ_LIBRARIES})
    endforeach()

    configure_file(${example}/deploy.xml.in "${CMAKE_CURRENT_BINARY_DIR}/${example}/deploy.xml")
	    
    ADD_TESH(${example} --setenv resources=${TEST_RESOURCES_DIR} --setenv bindir=${RSG_BINARY_DIR} --setenv bintestdir=${CMAKE_CURRENT_BINARY_DIR}/${example} ${CMAKE_CURRENT_SOURCE_DIR}/${example}/${example}.tesh)
endforeach()

#cmake don't rebuild executables when you change tests:
#http://stackoverflow.com/questions/733475/cmake-ctest-make-test-doesnt-build-tests
#(and yes for cmake devs it's a feature, not a bug!)
#you have to do make;ctest when developping your tests!
set(TEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## Add your new test directories here:
add_subdirectory(actor/basic)
add_subdirectory(actor/externalActor)
add_subdirectory(actor/fork)
add_subdirectory(actor/fork_from_spawned_actor)
add_subdirectory(actor/spawnActor)
add_subdirectory(actor/getPPID)
add_subdirectory(actor/joinActorTrivial)
add_subdirectory(actor/joinActor)
add_subdirectory(actor/dataActor)
add_subdirectory(actor/killActor)
add_subdirectory(actor/killActorPid)
add_subdirectory(comm/async_send_receive)
add_subdirectory(comm/async_send_receive_data_type)
add_subdirectory(comm/async_with_test)
add_subdirectory(comm/async_wait_any)
add_subdirectory(comm/async_wait_any2)
add_subdirectory(comm/async_wait_any_for)
add_subdirectory(comm/async_wait_any_for_empty)
add_subdirectory(comm/sync_async_combined)
add_subdirectory(conditionVariable/producerConsumer)
add_subdirectory(conditionVariable/wait_for)
add_subdirectory(conditionVariable/notify)
add_subdirectory(global/clock)
add_subdirectory(global/KVS)
add_subdirectory(host/prop)
add_subdirectory(mailbox/empty)
add_subdirectory(mailbox/send_receive)
add_subdirectory(mailbox/send_receive_data_type)
add_subdirectory(mailbox/two_way_comm)
add_subdirectory(mailbox/setReceiver)
add_subdirectory(mutex/basic)
add_subdirectory(turnOffTurnOn)
