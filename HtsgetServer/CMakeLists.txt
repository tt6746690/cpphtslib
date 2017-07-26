project(HtsgetServer)

# asio include 
set(ASIO_INCLUDE ${PROJECT_SOURCE_DIR}/lib/asio-1.10.6/include)

# set up library 
add_library(asio INTERFACE)
target_sources(asio INTERFACE ${ASIO_INCLUDE}/asio.hpp)
target_include_directories(asio INTERFACE
    "${ASIO_INCLUDE}"
)

# include directory
include_directories(include)
include_directories(SYSTEM ${ASIO_INCLUDE})

# source files
set(SOURCE_FILES 
    src/server.cpp 
    include/server.h
)

# compiling and linking
add_executable(server ${SOURCE_FILES})
target_link_libraries(server asio)

# find_package(Boost 1.64.0 REQUIRED COMPONENTS system regex thread date_time)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     add_executable(server ${SOURCE_FILES})
#     target_link_libraries(server ${Boost_LIBRARIES} htslib)
# endif()