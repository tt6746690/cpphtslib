# version and project name
cmake_minimum_required(VERSION 2.6)
project(HtsgetServer)

# set(ASIO_INCLUDE lib/asio-1.10.6/include)  -L${ASIO_INCLUDE}

# compiler   
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libstdc++ -Wall")
set(CMAKE_CXX_COMPILER_ID "Clang")

# generic path 
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)      
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})        

# configure boost 
# set(Boost_USE_STATIC_LIBS OFF) 
# set(Boost_USE_MULTITHREADED ON)  
# set(Boost_USE_STATIC_RUNTIME OFF) 


# add subdirectories 
add_subdirectory(${PROJECT_SOURCE_DIR}/HtsgetServer)
# add_subdirectory(${PROJECT_SOURCE_DIR}/htslib)
# add_subdirectory(${PROJECT_SOURCE_DIR}/learn)


# cmake -H. -Bbuild 
# cmake --build build -- -j3
# ./bin/server
