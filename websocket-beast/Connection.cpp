//
// Created by AdminCZ on 2026/5/1.
//

#include "Connection.h"
#include "Connection_Manager.h"

#include <boost/uuid/uuid_io.hpp>
#include <print>


Connection::Connection(boost::asio::io_context& ioc) :
    m_ioc(ioc),
    m_ws(boost::asio::make_strand(m_ioc)),
    m_uuid(boost::uuids::to_string(boost::uuids::random_generator()()))
{

}

boost::asio::ip::tcp::socket& Connection::GetSocket() noexcept
{
    return boost::beast::get_lowest_layer(m_ws).socket();
}

const std::string& Connection::GetUUID() const noexcept
{
    return m_uuid;
}

void Connection::AsyncConnection()
{
    /// 前提是已经和对端建立 HTTP 连接
    /// 用于读取 WebSocket HTTP Upgrade 请求握手，然后发送 WebSocket HTTP Upgrade 响应
    m_ws.async_accept([self = shared_from_this()](const boost::beast::error_code& ec) {
        if (ec) {
            std::println(stderr, "{}", ec.what());
            return;
        }

        Connection_Manager::GetInstance().AddConnection(self);
        self->Start();
    });
}

void Connection::Start()
{
    m_ws.async_read(
        m_buffer, [self = shared_from_this()](const boost::beast::error_code& ec, std::size_t bytes_transferred) {
            if (ec) {
                std::println(stderr, "{}", ec.what());
                Connection_Manager::GetInstance().RemoveConnection(self->GetUUID());
                return;
            }

            auto recv_str = boost::beast::buffers_to_string(self->m_buffer.data());
            self->m_buffer.consume(self->m_buffer.size());
            std::println("recv: {}", recv_str);
            self->m_ws.text(self->m_ws.got_text());
            self->Send(std::move(recv_str));
            self->Start();
        });
}

void Connection::Send(std::string&& data)
{
    std::lock_guard<std::mutex> lock(m_send_que_mtx);
    m_send_que.emplace(std::move(data));

    if (m_send_que.size() > 1) {
        return;
    }

    auto front = std::move(m_send_que.front());
    m_send_que.pop();

    Async_Write(std::move(front));
}

void Connection::Async_Write(std::string&& data)
{
    m_ws.async_write(boost::asio::buffer(data, data.size()),
                     [self = shared_from_this()](const boost::beast::error_code& ec, std::size_t bytes_transferred) {
                         boost::ignore_unused(bytes_transferred);

                         if (ec) {
                             std::println(stderr, "{}", ec.what());
                             Connection_Manager::GetInstance().RemoveConnection(self->GetUUID());
                             return;
                         }

                         std::lock_guard<std::mutex> lock(self->m_send_que_mtx);
                         if (!self->m_send_que.empty()) {
                             auto front = std::move(self->m_send_que.front());
                             self->m_send_que.pop();
                             self->Async_Write(std::move(front));
                         }
                     });
}


