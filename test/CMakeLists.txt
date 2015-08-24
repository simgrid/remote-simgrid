add_executable(dumb_client dumb_client.cpp)
target_link_libraries(dumb_client rsg)

add_executable(dumb_server dumb_server.cpp)
target_link_libraries(dumb_server rsg)
