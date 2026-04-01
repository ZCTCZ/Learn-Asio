//
// Created by AdminCZ on 2026/3/31.
//
#include "Header/EndPoint.h"

#include <iostream>
#include <string>
#include <boost/asio.hpp>
/**
 * @brief 生成客户端的终端结点（目标IP + 目标端口 + 协议类型）
 * @return
 */
int client_generate_endpoint()
{
    std::string row_ip_str = "180.101.51.73"; // 原始IP字符串
    unsigned short port_num = 443; // 端口
    boost::system::error_code ec; // 错误码
    boost::asio::ip::address ip_address = boost::asio::ip::make_address(row_ip_str, ec);
    if (0 != ec.value())
    {
        std::cout << "Fail to parse the IP [" << row_ip_str << "]!"
            << " error_code = " << ec.value()
            << " error_message = " << ec.message() << std::endl;
        return ec.value();
    }

    // 创建客户端的EndPoint
    boost::asio::ip::tcp::endpoint ep{ip_address, port_num};
    return 0;
}

/**
 * @brief 生成服务端的终端结点
 * @return
 */
int server_generate_endpoint()
{
    auto ip_address = boost::asio::ip::address_v6::any(); //服务器监听的是 IPv6 的“任意地址”
    unsigned short port_num = 443;
    boost::asio::ip::tcp::endpoint ep{ip_address, port_num};
    return 0;
}

/**
 * @brief 生成通信套接字
 * @return
 */
int create_tcp_socket()
{
    boost::asio::io_context ioc; // Asio 的 I/O 执行上下文
    auto protocol = boost::asio::ip::tcp::v4(); // 选择通信协议：tcp + ipv4
    boost::asio::ip::tcp::socket st(ioc); // 基于 ioc 构建一个 socket。如果构造的时候传入了 protocol,那么就在构造的时候自动open，后续无需再open
    boost::system::error_code ec;
    st.open(protocol, ec); // 调用 open 函数才真正的在底层创建 socket 通信套接字
    if (0 != ec.value())
    {
        std::cout << "Fail to open the socket!"
            << " error_code = " << ec.value()
            << " error_message = "
            << ec.message() << std::endl;
        return ec.value(); // 返回错误码
    }

    return 0;
}

/**
 * @brief 创建一个 acceptor（创建用于监听的socket + bind + listen）
 * @return
 */
int create_tcp_acceptor()
{
    boost::asio::io_context ioc; // 上下文服务
    auto protocol = boost::asio::ip::tcp::v6(); // 协议类型
    boost::asio::ip::tcp::acceptor acceptor(ioc); // 创建 acceptor
    boost::system::error_code ec;
    acceptor.open(protocol, ec); // 在底层创建一个监听套接字
    acceptor.set_option(boost::asio::ip::v6_only(false)); // 同时接收 ipv4 和 ipv6 的请求

    if (0 != ec.value())
    {
        std::cout << "Failed to open acceptor socket!"
            << " error_code = " << ec.value()
            << " error_message = "
            << ec.message();
        return ec.value();
    }

    // 创建服务端的 EndPoint
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v6::any(), 443);
    acceptor.bind(ep, ec); // 将 acceptor 绑定到本地 EndPoint
    if (0 != ec.value())
    {
        std::cout << "Acceptor bind to local EndPoint occurred error"
            << " error_code = " << ec.value()
            << " error_message = "
            << ec.message();
        return ec.value();
    }

    acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (0 != ec.value())
    {
        std::cout << "Acceptor to listen occurred error"
            << " error_code = " << ec.value()
            << " error_message = "
            << ec.message();
        return ec.value();
    }

    return 0;
}

/**
 * @brief 和服务器建立连接
 * @return
 */
int client_connect_to_server()
{
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address("180.101.51.73"),
                                          static_cast<unsigned short>(443));
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc);
        socket.connect(ep); // 向服务器发起连接请求。在此之前会自动调用 open 函数，在底层创建 socket 通信套接字
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
    }

    return 0;
}

int client_dns_connect_to_server()
{
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::resolver resolver(ioc); // 域名解析器

    try
    {
        auto ep = resolver.resolve("www.baidu.com", "443");
        boost::asio::connect(socket, ep);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
    }

    return 0;
}

void use_const_buf()
{
    const std::string str("Hello");
    boost::asio::const_buffer const_buf(str.c_str(), str.size());
    std::vector<boost::asio::const_buffer> vec;
    vec.emplace_back(const_buf);
}

void use_buf_str()
{
    std::string mutable_str("Hello");
    const std::string const_str("World");
    boost::asio::const_buffer const_buf = boost::asio::buffer(const_str);
    boost::asio::mutable_buffer mutable_buf = boost::asio::buffer(mutable_str);
    // std::cout << typeid(const_buf).name() << std::endl; //N5boost4asio12const_bufferE
    // std::cout << typeid(mutable_buf).name() << std::endl; //N5boost4asio14mutable_bufferE
}

void use_buf_array()
{
    constexpr ssize_t BUF_SIZE = 1024;
    std::unique_ptr<char[]> ptr_char(new char[BUF_SIZE]);
    boost::asio::mutable_buffer buf = boost::asio::buffer(ptr_char.get(), BUF_SIZE);
}

void write_some_to_socket(boost::asio::ip::tcp::socket& socket)
{
    const std::string data = "Hello World";
    ssize_t total_write = 0; // 已经发送的数据量
    boost::system::error_code ec;
    while (total_write != data.size())
    {
        /**
         * write_some() 函数可能只发送部分数据
         * 返回实际发送的字节数
         * 需要通过while 循环才能保证所有数据都发送出去
         */
        auto cur_write_bytes = socket.write_some(boost::asio::buffer(data.data() + total_write,
                                                                     data.size() - total_write), ec);
        if (ec)
        {
            std::cout << "Write Data Error"
                << "error_code = " << ec.value()
                << "error_message = " << ec.message() << std::endl;
            return;
        }

        total_write += cur_write_bytes;
    }
}

int send_data_by_write_some()
{
    const std::string server_ip_str = "180.101.51.73";
    const unsigned short server_port_num = 443;
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(server_ip_str), server_port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);
        write_some_to_socket(socket);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}

int send_data_by_send()
{
    const std::string server_ip_str = "180.101.51.73";
    const unsigned short server_port_num = 443;
    const std::string data = "Hello World";
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(server_ip_str), server_port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);
        boost::system::error_code ec;
        /**
         * send() 函数一次性发送完整的数据，可能只发送部分数据（缓冲区已满）
         * 返回值 > 0 :实际发送的字节数
         */
        auto cur_send_bytes = socket.send(boost::asio::buffer(data), 0, ec);
        if (ec)
        {
            std::cout << "Write Data Error"
                << "error_code = " << ec.value()
                << "error_message = " << ec.message() << std::endl;
            return ec.value();
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}

int send_data_by_write()
{
    const std::string server_ip_str = "180.101.51.73";
    const unsigned short server_port_num = 443;
    const std::string data = "Hello World";
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(server_ip_str), server_port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);
        boost::system::error_code ec;
        /**
         * write() 函数会保证发送完整的数据（内部循环执行 send() 函数）
         * 返回值 > 0 :实际发送的字节数，一定等于数据总长度
         */
        auto cur_write_bytes = boost::asio::write(socket, boost::asio::buffer(data), ec);
        if (ec)
        {
            std::cout << "Write Data Error"
                << "error_code = " << ec.value()
                << "error_message = " << ec.message() << std::endl;
            return ec.value();
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}

std::string read_some_from_socket(boost::asio::ip::tcp::socket& socket)
{
    constexpr ssize_t BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    ssize_t total_read = 0; // 总共读取到的数据字节数
    boost::system::error_code ec;

    while (total_read < BUF_SIZE)
    {
        /**
         * read_some() 函数用于从内核的读缓冲区读取数据
         * 返回值 > 0：本次接收到的数据字节数
         */
        auto cur_read = socket.read_some(boost::asio::buffer(buffer + total_read, BUF_SIZE - total_read), ec);
        if (ec)
        {
            if (ec == boost::asio::error::eof)
            {
                std::cout << "Opposite End Close" << std::endl;
                break;
            }
            std::cout << "Read Data Error"
                << "error_code = " << ec.value()
                << "error_message = " << ec.message() << std::endl;
            return std::string();
        }

        total_read += cur_read;
    }

    return std::string(buffer, total_read); // 返回接收到的数据
}

int read_data_by_read_some()
{
    std::string server_ip_str = "180.101.51.73";
    unsigned short server_port_num = 443;

    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(server_ip_str), server_port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);
        read_some_from_socket(socket);
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}

int read_data_by_receive()
{
    std::string server_ip_str = "180.101.51.73";
    unsigned short server_port_num = 443;

    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(server_ip_str), server_port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);
        constexpr ssize_t BUF_SIZE = 1024;
        char buffer[BUF_SIZE];
        boost::system::error_code ec;

        /**
         * receive() 函数从内核的读缓冲区读取数据，返回读取到的数据的字节数
         * 返回值 > 0:读取到的字节数
         */
        auto receive_bytes = socket.receive(boost::asio::buffer(buffer, BUF_SIZE), 0, ec);
        if (ec)
        {
            if (ec == boost::asio::error::eof)
            {
                std::cout << "Opposite End Close" << std::endl;
            }
            else
            {
                std::cout << "Receive Data Error"
                    << " error_code = " << ec.value()
                    << " error_message = " << ec.message() << std::endl;
                return ec.value();
            }
            return 0;
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}

int read_data_by_read()
{
    std::string server_ip_str = "180.101.51.73";
    unsigned short server_port_num = 443;

    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(server_ip_str), server_port_num);
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);
        constexpr ssize_t BUF_SIZE = 1024;
        char buffer[BUF_SIZE];
        boost::system::error_code ec;

        /**
         * read() 函数从内核的读缓冲区读取指定数目的数据，返回读取到的数据的字节数
         * 知道读满缓冲区或者发生错误，函数才会返回
         * 返回值 > 0:读取到的字节数，一定等于缓冲区的大小
         */
        auto read_bytes = boost::asio::read(socket, boost::asio::buffer(buffer, BUF_SIZE), ec);
        if (ec)
        {
            if (ec == boost::asio::error::eof)
            {
                std::cout << "Opposite End Close" << std::endl;
            }
            else
            {
                std::cout << "Read Data Error"
                    << "error_code = " << ec.value()
                    << "error_message = " << ec.message() << std::endl;
                return ec.value();
            }
            return 0;
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occurred!"
            << "error_code = " << e.code()
            << "error_message = " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}


