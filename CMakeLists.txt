# version and project name
cmake_minimum_required(VERSION 2.6)
project(HtsgetServer)

# compiler
set(CMAKE_CXX_COMPILER_ID "Clang")
# set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm@4/bin/clang-4.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++ -Wall")

# generic path 
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)      
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})        

# configure boost 
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

# add subdirectories 
add_subdirectory(${PROJECT_SOURCE_DIR}/HtsgetServer)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)


# cmake -H. -Bbuild -Wno-dev
# cmake --build build -- -j3
# ./bin/server
