//
// Created by AdminCZ on 2026/5/2.
//

#ifndef WEBSOCKET_BEAST_LISTENER_H
#define WEBSOCKET_BEAST_LISTENER_H
#include <boost/asio.hpp>

class Listener {
public:
    explicit Listener(boost::asio::io_context& ioc);
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;
    ~Listener() = default;
    void StartAccept();

private:
    boost::asio::io_context& m_ioc;
    boost::asio::ip::tcp::acceptor m_acceptor;
};


#endif //WEBSOCKET_BEAST_LISTENER_H
