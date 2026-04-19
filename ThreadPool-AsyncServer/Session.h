//
// Created by AdminCZ on 2026/4/4.
//

#ifndef ASYNCSERVER_SESSION_H
#define ASYNCSERVER_SESSION_H

#include <queue>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "LogicSystem.h"

/// 前向声明，避免循环包含
/// 不需要 include <Server.h>
class Server;
class SendNode;
class RecvNode;

/// Session 类继承自模板类 std::enable_shared_from_this<>
class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(boost::asio::io_context& ioc, Server* server_ptr);
    void start();
    void Send(const char* msg, MSG_LEN_TYPE len, MSG_TYPE id);
    void Send(const std::string& msg, MSG_TYPE id);
    boost::asio::ip::tcp::socket& Socket();
    const std::string& GetUUID() const;

private:
    /// 发送数据之后回调该函数
    void handle_write(const boost::system::error_code& ec);

    /// 读取消息首部之后回调该函数
    void handle_read_head(const boost::system::error_code& ec, size_t transfer_bytes);

    /// 读取消息体之后回调该函数
    void handle_read_body(const boost::system::error_code& ec, size_t transfer_bytes);

    boost::asio::ip::tcp::socket m_socket;
    // char m_data[BUF_SIZE] = {}; // 用户缓冲区，用来存放从对端发送过来的数据
    std::string m_uuid; // 当前 session 的唯一标志

    /// 不可以使用 shared_ptr，会造成循环引用问题
    /// 在现代 C++ 里，裸指针表达的就是 “非拥有引用” 的关系
    Server* m_server_ptr;

    std::queue<std::shared_ptr<SendNode>> m_MsgNode_que; // 待发送的数据的缓存队列
    std::mutex m_send_que_mtx; // 用来维护 m_MsgNode_que 并发访问的互斥量
    std::atomic<bool> m_pending{false}; //  记录上一个 MsgNode 有没有发送完成

    std::shared_ptr<RecvNode> m_recv_head; // 存放当前消息的消息头
    std::shared_ptr<RecvNode> m_recv_body; // 存放接收到的一条消息的消息体
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand; // 管理每个session对象的回调任务
};


#endif //ASYNCSERVER_SESSION_H
