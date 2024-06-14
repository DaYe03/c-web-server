cmake_minimum_required(VERSION 3.2)
project(cwebserver)

set(SOURCES
    src/client.c
    src/http_data.c
    src/linked_list.c
    src/process_request.c
    src/process_response.c
    src/route.c
    src/server.c
)

# Create the library from the source files
add_library(cwebserver ${SOURCES})

# Set the public header file
set_target_properties(cwebserver PROPERTIES 
    PUBLIC_HEADER "src/server.h;src/route.h;src/http_data.h"
    PRIVATE_HEADER "src/client.h;src/linked_list.h;src/process_request.h;src/process_response.h"
)

# Specify installation locations for the library and header file
install(TARGETS cwebserver
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    PRIVATE_HEADER DESTINATION include
)

# Add the executable for demo
add_executable(demo demo/main.c)

# Link the library to the executable
target_link_libraries(demo cwebserver)