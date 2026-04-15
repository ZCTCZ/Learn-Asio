#include <iostream>

#include "Server.h"

int main()
{
    try
    {
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM, SIGABRT);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_num)
        {
            if (error)
            {
                std::cerr << "Occurred Error When Capture Signal!"
                    << " error_code = " << error.value()
                    << " error_message = " << error.message() << std::endl;
            }
            else
            {
                std::cout << "Capture Signal:" << signal_num << '\n'
                    << "服务器完美退出！" << std::endl;
            }

            ioc.stop();
        });
        
        Server s(ioc, 60001);
        ioc.run(); // 启动底层的 I/O 多路复用
    }
    catch (boost::system::system_error& error)
    {
        std::cerr << "Occurred Error"
            << " error_code = " << error.code().value()
            << " error_message = " << error.what() << std::endl;
    }
    return 0;
}
