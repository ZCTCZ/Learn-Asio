cmake_minimum_required(VERSION 4.2)
project(AsyncAPI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(AsyncAPI main.cpp
        Session.cpp)

target_include_directories(AsyncAPI PRIVATE Header/*)
target_link_libraries(AsyncAPI PRIVATE ws2_32)
