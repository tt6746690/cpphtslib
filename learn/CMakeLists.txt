project(boost_example_code)

include_directories(code)

find_package(Boost 1.64.0 REQUIRED COMPONENTS system regex thread date_time)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(timer.5 code/timer.5.cpp)
target_link_libraries(timer.5 ${Boost_LIBRARIES})