project(HtsgetServer)

include_directories(include)
include_directories(SYSTEM ${HEADER_ONLY_INCLUDE})
include_directories(${Http_SOURCE_DIR}/include)

set(SOURCE_FILES 
    Config.h
    Error.h 
    Ticket.h 
    Wrapper.h
)

add_executable(htsgetserver main.cpp ${SOURCE_FILES})
target_link_libraries(htsgetserver Http)

