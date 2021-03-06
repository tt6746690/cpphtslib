# cmake -Bbuild Wno-dev
# cmake --build build -- -j5

# version and project name
cmake_minimum_required(VERSION 2.6)
project(Htsget)

# compiler
set(CMAKE_CXX_COMPILER_ID "Clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++ -Wall  -I/usr/local/opt/openssl/include")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/openssl/lib -lssl -lcrypto")

# generic path 
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)      
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})        

# configure boost 
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

# header only libraries libraries 
set(HEADER_ONLY_INCLUDE 
    ../lib/asio-1.10.6/include
    ../lib/Catch/single_include
    ../lib/json/src)


# add subdirectories 
set(Http_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Http)
set(Lib_SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib)
set(Server_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Server)
add_subdirectory(${Http_SOURCE_DIR})
add_subdirectory(${Lib_SOURCE_DIR})
add_subdirectory(${Server_SOURCE_DIR})


# set(Learn_SOURCE_DIR ${PROJECT_SOURCE_DIR}/learn)
# add_subdirectory(${Learn_SOURCE_DIR})


