project(boost_example_code)

include_directories(code)
include_directories(code/ssl)
include_directories(SYSTEM ${HEADER_ONLY_INCLUDE})


add_executable(timer code/timer.cpp)
add_executable(TCP_daytime_server code/TCP_daytime_server.cpp)

add_executable(sslserver code/ssl/server.cpp)
add_executable(sslclient code/ssl/client.cpp)




