cmake_minimum_required (VERSION 2.8.1)

add_executable(SDP_serv src/server.c src/state.c)
target_link_libraries(SDP_serv pthread)
