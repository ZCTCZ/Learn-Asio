#include "Server.h"

#include <print>

int main()
{
    try {
        Server server{boost::asio::ip::address_v4::any().to_string(), 60001};
        server.Run();
    }
    catch (std::exception& e) {
        std::println("Occurred Error! Error Message = {}", e.what());
    }
    
    return 0;
}
