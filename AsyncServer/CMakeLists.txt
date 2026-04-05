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

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif ()
