//
// Created by AdminCZ on 2026/4/4.
//

#include "Server.h"

#include <iostream>

Server::Server(boost::asio::io_context& ioc, unsigned short port)
    :m_ioc(ioc),
     m_acceptor([&ioc, port]
     {
         try
         {
             // creates an acceptor and automatically opens it to listen
             // boost::system::system_error Thrown on failure.
             return boost::asio::ip::tcp::acceptor(ioc,
                 boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port));
         }
         catch (const boost::system::system_error &ec)
         {
             std::cerr << "Create Acceptor Error"
             << " error_code = " << ec.code().value()
             << " error_message = " << ec.what() << std::endl;
             throw;
         }
     }())
{
    std::cout << "Run Server Successfully, port = " << port << std::endl;
    start_accept();
}

void Server::start_accept()
{
    auto new_session = new Session(m_ioc);
    m_acceptor.async_accept(new_session->Socket(),
        std::bind(&Server::handle_accept, this, std::placeholders::_1, new_session));
}

void Server::handle_accept(const boost::system::error_code& ec, Session* new_session)
{
    if (ec)
    {
        std::cerr << "Accept Connection Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        delete new_session;
        return;
    }

    std::cout << "New Connection : " << new_session->Socket().remote_endpoint().address().to_string() << std::endl;

    // 处理新连接的 I/O
    new_session->start();

    start_accept();
}
