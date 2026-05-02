//
// Created by AdminCZ on 2026/5/2.
//

#include "Listener.h"
#include  <print>

#include "Connection.h"

Listener::Listener(boost::asio::io_context& ioc) :
    m_ioc(ioc), m_acceptor(m_ioc)
{
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address_v4::any(), 60001};
    m_acceptor.open(ep.protocol(), ec);
    if (ec) {
        std::println(stderr, "{}", ec.what());
        return;
    }

    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec) {
        std::println(stderr, "{}", ec.what());
        return;
    }

    m_acceptor.bind(ep, ec);
    if (ec) {
        std::println(stderr, "{}", ec.what());
        return;
    }

    m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        std::println(stderr, "{}", ec.what());
        return;
    }
}

void Listener::StartAccept()
{
    auto new_conn = std::make_shared<Connection>(m_ioc);
    m_acceptor.async_accept(new_conn->GetSocket(),
                            [this, new_conn](const boost::system::error_code& error) {
                                if (error) {
                                    std::println(stderr, "{}", error.what());
                                }
                                else {
                                    new_conn->AsyncConnection();
                                }

                                this->StartAccept();
                            });
}


