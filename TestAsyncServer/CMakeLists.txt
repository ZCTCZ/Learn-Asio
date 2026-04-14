cmake_minimum_required(VERSION 4.2)
project(TestAsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(TestAsyncServer main.cpp)

target_link_libraries(TestAsyncServer PRIVATE ws2_32)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(TestAsyncServer PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(TestAsyncServer PUBLIC /source-charset:UTF-8 /execution-charset:GBK)
endif()
