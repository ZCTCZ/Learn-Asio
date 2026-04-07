cmake_minimum_required(VERSION 4.2)
project(TestAsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(TestAsyncServer main.cpp)

target_link_libraries(TestAsyncServer PRIVATE ws2_32)

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif ()