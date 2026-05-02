cmake_minimum_required(VERSION 4.2)
project(websocket_beast)

set(CMAKE_CXX_STANDARD 23)

add_executable(websocket_beast main.cpp
        Connection.cpp
        Connection_Manager.cpp
        Listener.cpp)

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(websocket_beast PUBLIC -finput-charset=UTF-8 -fexec-charset=GBK)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(websocket_beast PUBLIC /source-charset:UTF-8 /execution-charset:GBK)
    endif ()

    target_link_libraries(websocket_beast PRIVATE ws2_32 mswsock)
endif ()
