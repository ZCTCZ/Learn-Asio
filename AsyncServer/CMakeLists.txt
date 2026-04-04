cmake_minimum_required(VERSION 4.2)
project(AsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(AsyncServer main.cpp
        Session.cpp
        Server.cpp)

target_link_libraries(AsyncServer PRIVATE ws2_32 mswsock)
