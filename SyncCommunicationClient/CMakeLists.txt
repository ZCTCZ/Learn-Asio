cmake_minimum_required(VERSION 4.2)
project(SyncCommunicationClient)

set(CMAKE_CXX_STANDARD 20)

add_executable(SyncCommunicationClient main.cpp)

target_link_libraries(SyncCommunicationClient PRIVATE ws2_32)
