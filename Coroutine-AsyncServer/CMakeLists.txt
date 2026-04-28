cmake_minimum_required(VERSION 4.2)
project(Coroutine_AsyncServer)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(Coroutine_AsyncServer main.cpp
        MsgNode.cpp
        RecvNode.cpp
        SendNode.cpp
        IOContextPool.cpp
        Server.cpp
        Session.cpp
        LogicSystem.cpp)

find_package(nlohmann_json CONFIG REQUIRED)

if (WIN32)
    target_link_libraries(Coroutine_AsyncServer PRIVATE ws2_32 mswsock nlohmann_json::nlohmann_json)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|CLANG")
    target_compile_options(Coroutine_AsyncServer PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(Coroutine_AsyncServer PUBLIC /source-charset:UTF-8 /execution-charset:GBK)
endif ()