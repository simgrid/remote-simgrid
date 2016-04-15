add_executable(main_client main_client.cpp)

# Libraries to link
target_link_libraries(main_client thriftClient)

target_link_libraries(main_client "RsgService.thrift")
target_link_libraries(main_client ${THRIFT_LIBRARIES})
target_link_libraries(main_client ${SimGrid_LIBRARY})
