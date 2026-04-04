//
// Created by AdminCZ on 2026/4/3.

#ifndef ASYNCAPI_SESSION_H
#define ASYNCAPI_SESSION_H
#include <cctype>
#include <cstring>
#include <memory>
#include <queue>
#include <string>
#include <boost/asio.hpp>
#include <utility>

class Session;
/**
 * @brief 数据结点类，封装了待发送或者接收到的数据
 */
class MsgNode
{
    friend  class Session;
public:
    static constexpr size_t MAXSIZE = 1024;
    /**
     * @brief 构造数据结点对象，存放待发送的数据
     * @param msg 待发送的数据
     * @param total_len 待发送的数据长度
     */
    MsgNode(const char *msg, const size_t total_len)
        :m_msg(new char[total_len]), m_total_len(total_len)
    {
        memcpy(m_msg, msg, total_len);
    }

    /**
     * @brief 构造数据结点对象，存放待接收的数据
     * @param total_len 指定接收空间的大小
     */
    explicit MsgNode(const size_t total_len)
        :m_total_len(total_len)
    {
    }

    ~MsgNode()
    {
        delete [] m_msg;
    }
private:
    char * m_msg = nullptr;
    size_t m_total_len = 0;
    size_t m_cur_len = 0; // 已读或者已写的长度
};

class Session
{
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr)
        :m_socket_ptr(std::move(socket_ptr)), m_read_node(nullptr)
    {
    }
    void Connect(const boost::asio::ip::tcp::endpoint &ep) const {m_socket_ptr->connect(ep);}

    void WriteSomeCallBackErr(boost::system::error_code ec, size_t transfer_bytes);
    void WriteSomeToSocketErr(const std::string &buf);
    void SendCallBackErr(boost::system::error_code ec, size_t transfer_bytes);
    void SendToSocketErr(const std::string &buf);
    void WriteCallBackErr(boost::system::error_code ec, size_t transfer_bytes);
    void WriteToSocketErr(const std::string &buf);

    void ReadSomeCallBackErr(boost::system::error_code ec, size_t transfer_bytes);
    void ReadSomeFromSocketErr();
    void ReceiveCallbackErr(boost::system::error_code ec, size_t transfer_bytes);
    void ReceiveFromSocketErr();
    void ReadCallBackErr(boost::system::error_code ec, size_t transfer_bytes);
    void ReadFromSocketErr();
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> m_socket_ptr;
    std::queue<std::shared_ptr<MsgNode>> m_send_que;
    bool m_send_pending = false; // 记录是否有异步写操作在执行
    bool m_read_pending = false; // 记录是否有异步读操作在执行
    std::shared_ptr<MsgNode> m_read_node;
};
#endif //ASYNCAPI_SESSION_H
