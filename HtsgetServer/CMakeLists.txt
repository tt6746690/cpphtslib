project(HtsgetServer)

# # asio include 
set(HEADER_ONLY_INCLUDE 
    ../lib/asio-1.10.6/include
    ../lib/Catch/single_include)

# # set up library 
# add_library(asio INTERFACE)
# target_sources(asio INTERFACE ${ASIO_INCLUDE}/asio.hpp)
# target_include_directories(asio INTERFACE
#     "${ASIO_INCLUDE}"
# )

# # include directory
include_directories(include)
include_directories(SYSTEM ${HEADER_ONLY_INCLUDE})

# server
set(SOURCE_FILES 
    main.cpp
    src/Server.cpp 
    include/Server.h
    include/Fib.h
)

add_executable(server ${SOURCE_FILES})


# test
set(TEST_FILES
    main-test.cpp 
    test/test_fib.cpp
    include/Fib.h
)

add_executable(test ${TEST_FILES})


# target_link_libraries(server lib)

# find_package(Boost 1.64.0 REQUIRED COMPONENTS system regex thread date_time)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     add_executable(server ${SOURCE_FILES})
#     target_link_libraries(server ${Boost_LIBRARIES} htslib)
# endif()