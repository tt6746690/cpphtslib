# version and project name
cmake_minimum_required(VERSION 2.6)
project(HtsgetServer)

# compiler 
set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

# version number 
set(HtsgetServer_VERSION_MAJOR 0)
set(HtsgetServer_VERSION_MINOR 1)

# generic path 
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)      
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})        
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)      


# add subdirectories 
add_subdirectory(${PROJECT_SOURCE_DIR}/HtsgetServer)
add_subdirectory(${PROJECT_SOURCE_DIR}/htslib)

# cmake -H. -Bbuild 
# cmake --build build -- -j3
# ./bin/server
