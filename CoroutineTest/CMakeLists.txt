cmake_minimum_required(VERSION 4.2)
project(CoroutineTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(CoroutineTest main.cpp)

if (WIN32)
    target_link_libraries(CoroutineTest PRIVATE ws2_32 mswsock)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|CLANG")
    target_compile_options(CoroutineTest PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(CoroutineTest PUBLIC /source-charset:UTF-8 /execution-charset:GBK)
endif ()