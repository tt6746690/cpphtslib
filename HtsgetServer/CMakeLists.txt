project(HtsgetServer)

# include directory
include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/htslib/include)

# source files
set(SOURCE_FILES 
    src/server.cpp 
    include/server.h
)


# boost 
find_package(Boost 1.64.0 COMPONENTS filesystem regex)
if(Boost_Found)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
    add_executable(server ${SOURCE_FILES})
    target_link_libraries(server ${Boost_LIBRARIES} htslib)
endif()

