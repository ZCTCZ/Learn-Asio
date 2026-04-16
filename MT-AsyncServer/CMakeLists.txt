cmake_minimum_required(VERSION 4.2)
project(AsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
#    # 开启标准库的编译选项
#    add_compile_definitions(_GLIBCXX_DEBUG)
#endif ()

add_executable(AsyncServer
        main.cpp
        Session.cpp
        Server.cpp
        MsgNode.cpp
        RecvNode.cpp
        SendNode.cpp
        LogicSystem.cpp
        Singleton.h
        IOServicePool.cpp)

target_link_libraries(AsyncServer PRIVATE ws2_32 mswsock)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(AsyncServer PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(AsyncServer PUBLIC /source-charset:utf-8 /execution-charset:GBK)
endif ()