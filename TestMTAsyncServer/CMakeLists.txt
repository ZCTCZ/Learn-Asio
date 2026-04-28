cmake_minimum_required(VERSION 4.2)
project(TestMTAsyncServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(TestMTAsyncServer main.cpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(TestMTAsyncServer PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(TestMTAsyncServer PUBLIC /source-charset:utf-8 /execution-charset:GBK)
endif ()

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(TestMTAsyncServer PRIVATE ws2_32 nlohmann_json::nlohmann_json)