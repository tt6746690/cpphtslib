project(htslib)

include_directories(include)

set(SOURCE_LIBS 
    src/foo.cpp 
    include/foo.h
)

add_library(htslib ${SOURCE_LIBS})