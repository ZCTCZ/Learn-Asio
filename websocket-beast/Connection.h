//
// Created by AdminCZ on 2026/5/1.
//

#ifndef WEBSOCKET_BEAST_CONNECTION_H
#define WEBSOCKET_BEAST_CONNECTION_H

#include <queue>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/uuid.hpp>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    explicit Connection(boost::asio::io_context& ioc);
    const std::string& GetUUID() const noexcept;
    boost::asio::ip::tcp::socket& GetSocket() noexcept;
    void Start();
    void AsyncConnection();

private:
    void Send(std::string&& data);
    void Async_Write(std::string&& data);
    boost::asio::io_context& m_ioc;
    boost::beast::websocket::stream<boost::beast::tcp_stream> m_ws;
    std::string m_uuid;
    boost::beast::flat_buffer m_buffer{}; // 读缓冲区
    std::queue<std::string> m_send_que;
    std::mutex m_send_que_mtx;
};


#endif //WEBSOCKET_BEAST_CONNECTION_H
