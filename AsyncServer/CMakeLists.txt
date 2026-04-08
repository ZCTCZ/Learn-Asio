cmake_minimum_required(VERSION 4.2)
project(AsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(AsyncServer
                            main.cpp
                            Session.cpp
                            Server.cpp
                            MsgNode.cpp)

target_link_libraries(AsyncServer PRIVATE ws2_32 mswsock)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(AsyncServer PRIVATE -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(AsyncServer PRIVATE /source-charset:utf-8 /execution-charset:GBK)
endif()