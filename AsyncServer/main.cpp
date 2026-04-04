#include <iostream>

#include "Server.h"

int main() {
    try
    {
        boost::asio::io_context ioc;
        Server s(ioc, 60001);
        ioc.run(); // 启动底层的 I/O 多路复用
    }
    catch (boost::system::system_error &error)
    {
        std::cerr << "Occurred Error"
        << " error_code = " << error.code().value()
        << " error_message = " << error.what() << std::endl;
    }
    return 0;
}
