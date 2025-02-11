cmake_minimum_required(VERSION 3.10)

project(WebserverProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${INC_DIR})

set(SOURCES
    ${SRC_DIR}/client_handler.c
    ${SRC_DIR}/server.c
    ${SRC_DIR}/socket.c
)

add_executable(server ${SOURCES})

target_link_libraries(server ws2_32)

target_include_directories(server PRIVATE ${INC_DIR})

target_compile_options(server PRIVATE -g -Wall)

target_sources(server PRIVATE ${OBJECTS})
