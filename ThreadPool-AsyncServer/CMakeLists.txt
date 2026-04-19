cmake_minimum_required(VERSION 4.2)
project(ThreadPool_AsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(ThreadPool_AsyncServer main.cpp
        MsgNode.cpp
        RecvNode.cpp
        SendNode.cpp
        Server.cpp
        Singleton.h
        IOThreadPool.cpp
        Session.cpp
        LogicSystem.cpp)
target_link_libraries(ThreadPool_AsyncServer PRIVATE ws2_32 mswsock)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|CLANG")
    target_compile_options(ThreadPool_AsyncServer PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ThreadPool_AsyncServer PUBLIC /source-charset:UTF-8 /execution-charset:GBK)
endif ()