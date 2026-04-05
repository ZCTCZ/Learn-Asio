//
// Created by AdminCZ on 2026/4/4.
//

#ifndef ASYNCSERVER_SERVER_H
#define ASYNCSERVER_SERVER_H

#include "Session.h"
#include <unordered_map>

class Server {
public:
    Server(boost::asio::io_context& ioc, unsigned short port);
    void clear_session(const std::string &uuid);
private:
    void start_accept();
    void handle_accept(const boost::system::error_code &ec, std::shared_ptr<Session> new_session);
    boost::asio::io_context& m_ioc;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_map<std::string, std::shared_ptr<Session>> m_session_map; // 管理已经建立的连接的session
};



#endif //ASYNCSERVER_SERVER_H
