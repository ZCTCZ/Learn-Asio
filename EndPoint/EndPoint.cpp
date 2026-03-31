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



