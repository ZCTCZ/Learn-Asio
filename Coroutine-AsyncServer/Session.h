//
// Created by AdminCZ on 2026/4/24.
//

#ifndef COROUTINE_ASYNCSERVER_SESSION_H
#define COROUTINE_ASYNCSERVER_SESSION_H

#include <queue>
#include <boost/asio/ip/tcp.hpp>
#include "constant.h"

class Server;
class RecvNode;
class SendNode;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::io_context& io_context, Server* server_ptr);
    ~Session();

    boost::asio::ip::tcp::socket& GetSocket() noexcept;
    const boost::asio::ip::tcp::socket& GetSocket() const noexcept;
    const std::string& GetUUID() const noexcept;
    void Set_Remote_Address(std::string remote_address);
    const std::string& Get_Remote_Address() const noexcept;
    void Run();
    bool IsClose() const noexcept;
    void Send(const char* msg, std::size_t msg_len, MSG_TYPE id);
    void Send(const std::string& msg, MSG_TYPE id);
    void Close();

private:
    void HandleWrite(const boost::system::error_code&, std::size_t);

    boost::asio::ip::tcp::socket m_socket;
    Server* m_server_ptr{nullptr};
    std::atomic<bool> m_closed{false}; // 表示当前会话是否关闭

    std::string m_uuid;
    std::string m_remote_address;

    std::shared_ptr<RecvNode> m_recv_head_node; // 存放消息头
    std::shared_ptr<RecvNode> m_recv_body_node; // 存放消息体

    std::queue<std::shared_ptr<SendNode>> m_send_que; // 存放待发送的消息
    std::mutex m_send_que_mtx;
    std::atomic<bool> m_pending{false}; // 表示是否存在未完成的异步发送任务
};


#endif //COROUTINE_ASYNCSERVER_SESSION_H
