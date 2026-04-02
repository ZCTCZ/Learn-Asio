#include <iostream>
#include <boost/asio.hpp>
constexpr ssize_t BUF_SIZE = 1024;
const std::string SERVER_IP_STR = "127.0.0.1"; // 服务器 IP 地址
constexpr unsigned short SERVER_PORT_NUM = 60001; // 服务器端口号

int main() {
    try
    {
        boost::asio::ip::tcp::endpoint server_ep(boost::asio::ip::make_address(SERVER_IP_STR), SERVER_PORT_NUM);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc);
        boost::system::error_code ec;
        socket.connect(server_ep, ec); // 在使用的时候才给 socket 绑定协议
        if (ec)
        {
            std::cerr << "Connect to "
            << server_ep.address().to_string() << ":" << server_ep.port()
            << " failed!" << std::endl;
            std::cerr << "error_code = " << ec.value()
            << " error_message = " << ec.message() << std::endl;
            return 0;
        }
        std::cout << "Connect to "
            << server_ep.address().to_string() << ":" << server_ep.port()
            << " successfully!" << std::endl;

        char request[BUF_SIZE] = {};

        std::cout << "Enter:";
        // 阻塞等待用户从控制台输出
        std::cin.getline(request, BUF_SIZE);

        size_t request_len = strlen(request);
        size_t write_bytes = boost::asio::write(socket, boost::asio::buffer(request, request_len), ec);
        if (ec)
        {
            std::cerr << "Send Message Error"
            << " error_code = " << ec.value()
            << " error_message = " << ec.message() << std::endl;
            return 0;
        }

        char reply[BUF_SIZE] = {};
        size_t read_bytes = socket.read_some(boost::asio::buffer(reply, BUF_SIZE), ec);
        if (ec)
        {
            std::cerr << "Send Message Error"
            << " error_code = " << ec.value()
            << " error_message = " << ec.message() << std::endl;
            return 0;
        }

        std::cout << server_ep.address() << " reply: ";
        std::cout.write(reply, read_bytes);
        std::cout << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << "Happen Exception: " << e.what()<< std::endl;
    }

    return 0;
}