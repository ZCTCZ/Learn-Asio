//
// Created by AdminCZ on 2026/4/4.
//

#ifndef ASYNCSERVER_SERVER_H
#define ASYNCSERVER_SERVER_H

#include "Session.h"

class Server {
public:
    Server(boost::asio::io_context& ioc, unsigned short port);
private:
    void start_accept();
    void handle_accept(const boost::system::error_code &ec, Session *new_session);
    boost::asio::io_context& m_ioc;
    boost::asio::ip::tcp::acceptor m_acceptor;
};



#endif //ASYNCSERVER_SERVER_H
