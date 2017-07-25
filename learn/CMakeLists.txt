project(boost_example_code)

include_directories(code)

find_package(Boost 1.64.0 REQUIRED COMPONENTS system regex thread date_time)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(timer code/timer.cpp)
add_executable(TCP_daytime_server code/TCP_daytime_server.cpp)


target_link_libraries(timer ${Boost_LIBRARIES})
target_link_libraries(TCP_daytime_server ${Boost_LIBRARIES})

