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

#include "MsgNode.h"

/// 前向声明，避免循环包含
/// 不需要 include <Server.h>
class Server;

class MsgNode;

/// Session 类继承自模板类 std::enable_shared_from_this<>
class Session : public std::enable_shared_from_this<Session>{
    friend class Server;
    static constexpr size_t BUF_SIZE = 1024; // 用户的读缓冲区最大大小
    static constexpr int MAX_LENGTH = 1024 * 8; // 单条数据大小不能超过 8k 字节
public:
    explicit Session(boost::asio::io_context& ioc, Server *server_ptr)
        : m_socket(ioc),// 根据传入的上下文构建 socket
          m_server_ptr(server_ptr),
          m_recv_head(std::make_shared<MsgNode>(MsgNode::HEAD_LENGTH))
    {
        const auto uuid = boost::uuids::random_generator()();
        m_uuid = boost::uuids::to_string(uuid);
    }

    void start();
    void Send(const char *msg, const size_t len);

    /**
     * @return boost::asio::ip::tcp::socket 不支持拷贝，返回引用
     */
    boost::asio::ip::tcp::socket& Socket() {return m_socket;}
private:
    void handle_read(const boost::system::error_code &ec, size_t transfer_bytes);
    void handle_write(const boost::system::error_code &ec);

    boost::asio::ip::tcp::socket m_socket;
    char m_data[BUF_SIZE] = {'\0'}; // 缓冲区，用来存放从对端发送过来的数据
    std::string m_uuid; // 当前 session 的唯一标志

    /// 不可以使用 shared_ptr，会造成循环引用问题
    /// 在现代 C++ 里，裸指针表达的就是 “非拥有引用” 的关系
    Server* m_server_ptr;

    /// 存放待发送的数据
    std::queue<std::shared_ptr<MsgNode>> m_MsgNode_que;
    std::mutex m_send_que_mtx; // 用来维护 m_MsgNode_que 并发访问的互斥量
    std::atomic<bool> m_pending{false}; //  记录上一个 MsgNode 有没有发送完成

    bool m_head_parsed = false; // 记录当前这条消息的消息头是否已经解析完成
    std::shared_ptr<MsgNode> m_recv_head; // 存放当前消息的消息头
    std::shared_ptr<MsgNode> m_recv_body; // 存放接收到的一条消息的消息体
};



#endif //ASYNCSERVER_SESSION_H
