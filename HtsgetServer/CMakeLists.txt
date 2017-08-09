project(HtsgetServer)

# cmake -H. -Bbuild -Wno-dev
# cmake --build build -- -j3
# ./bin/server

# header only libraries libraries 
set(HEADER_ONLY_INCLUDE 
    ../lib/asio-1.10.6/include
    ../lib/Catch/single_include
    ../lib/sparsepp)

# # include directory
include_directories(include)
include_directories(SYSTEM ${HEADER_ONLY_INCLUDE})

# server
set(SOURCE_FILES 
    src/Server.cpp 
    src/Connection.cpp
    src/Message.cpp
    src/Request.cpp
    src/RequestParser.cpp
    src/Response.cpp
    src/Utilities.cpp
    src/Uri.cpp
    src/Router.cpp
    include/Server.h
    include/Connection.h 
    include/Message.h 
    include/Request.h 
    include/RequestParser.h 
    include/Response.h 
    include/Utilities.h
    include/Uri.h
    include/Constants.h
    include/Router.h
    include/Trie.h
)

add_executable(server main.cpp ${SOURCE_FILES})


# test
set(TEST_FILES
    test/test_RequestParser.cpp
    test/test_Message.cpp
    test/test_Response.cpp
    test/test_Trie.cpp
    test/test_Router.cpp
    test/test_Uri.cpp
)

add_executable(test main-test.cpp  ${TEST_FILES} ${SOURCE_FILES})


# target_link_libraries(server lib)

# find_package(Boost 1.64.0 REQUIRED COMPONENTS system regex thread date_time)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     add_executable(server ${SOURCE_FILES})
#     target_link_libraries(server ${Boost_LIBRARIES} htslib)
# endif()