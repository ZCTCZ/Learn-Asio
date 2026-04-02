cmake_minimum_required(VERSION 4.2)
project(SyncCommunicationServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(SyncCommunicationServer main.cpp)

target_link_libraries(SyncCommunicationServer PRIVATE ws2_32)
