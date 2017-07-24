# include directory
include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/htslib/include)

# source files
set(SOURCE_FILES 
    src/server.cpp 
    include/server.h
)

# add executable 
add_executable(server ${SOURCE_FILES})

# link with htslibs 
target_link_libraries(server htslib)

