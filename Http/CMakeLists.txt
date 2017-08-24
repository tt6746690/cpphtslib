project(Http)

# cmake -H. -Bbuild -Wno-dev
# cmake --build build -- -j3

# include directory and set up lib dependencies
include_directories(include)
include_directories(SYSTEM ${HEADER_ONLY_INCLUDE})

# openssl
find_package(OpenSSL)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

# server
set(SOURCE_FILES 
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
    include/Codec.h
    src/Server.cpp 
    src/Connection.cpp
    src/Message.cpp
    src/RequestParser.cpp
    src/Response.cpp
    src/Uri.cpp
)

add_executable(server main.cpp ${SOURCE_FILES})
add_library(Http ${SOURCE_FILES})

# test
set(TEST_FILES
    test/test_RequestParser.cpp
    test/test_Message.cpp
    test/test_Response.cpp
    test/test_Trie.cpp
    test/test_Router.cpp
    test/test_Uri.cpp
    test/test_Server.cpp
    test/test_Codec.cpp
)

add_executable(testt main-test.cpp  ${TEST_FILES} ${SOURCE_FILES})

