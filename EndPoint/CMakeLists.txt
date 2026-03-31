cmake_minimum_required(VERSION 4.2)
project(EndPoint)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(EndPoint main.cpp EndPoint.cpp)
target_link_libraries(EndPoint PRIVATE ws2_32)

