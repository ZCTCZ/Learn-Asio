//
// Created by AdminCZ on 2026/4/4.
//

#include "Session.h"
#include "Server.h"
#include <iostream>

#include "MsgNode.h"


void Session::start()
{
    m_socket.async_read_some(boost::asio::buffer(m_data, Session::BUF_SIZE),
        std::bind(&Session::handle_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::Send(const char *msg, const size_t len)
{
    {
        std::lock_guard<std::mutex> lock(m_send_que_mtx);
        m_MsgNode_que.push(std::make_shared<MsgNode>(msg, len));

        if (m_pending) //上次还有 MsgNode 未发送完。将这次待发送的 MsgNode 添加到发送队列里即可
        {
            return ;
        }

        boost::asio::async_write(m_socket, boost::asio::buffer(msg, len),
            std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
        m_pending.store(true);
    }
}

void Session::handle_read(const boost::system::error_code& ec, size_t transfer_bytes)
{
    if (ec)
    {
        /// EOF是一种持久的状态，会存在于缓冲区里一段时间，可以被多个异步读操作读取到
        if (ec == boost::asio::error::eof)
        {
            std::cout << m_socket.remote_endpoint().address().to_string() << " disconnected" << std::endl;
            /// 销毁当前 Session 对象
            m_server_ptr->clear_session(m_uuid);
            return;
        }

        std::cerr << "Occurred Error When Read"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;

        /// 销毁当前 Session 对象
        m_server_ptr->clear_session(m_uuid);
        return;
    }

    /// 打印客户端发送过来的数据
    std::cout << "Read Data From " << m_socket.remote_endpoint().address().to_string() << " : [";
    std::cout.write(m_data, transfer_bytes);
    std::cout << "]" << std::endl;

    Send(m_data, transfer_bytes); // 调用 Send() 接口将接受到的数据发送给对端

    memset(m_data, '\0', Session::BUF_SIZE);

    ///继续调用 async_read_some，读取对端发送过来的数据
    m_socket.async_read_some(boost::asio::buffer(m_data, Session::BUF_SIZE),
        std::bind(&Session::handle_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::handle_write(const boost::system::error_code& ec)
{
    if (ec)
    {
        std::cerr << "Occurred Error When Write"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;

        /// 销毁当前 Session 对象
        m_server_ptr->clear_session(m_uuid);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_send_que_mtx);
        m_MsgNode_que.pop();

        if (!m_MsgNode_que.empty())
        {
            const auto node = m_MsgNode_que.front();
            boost::asio::async_write(m_socket, boost::asio::buffer(node->m_data, node->m_max_len),
                std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
            return;
        }

        m_pending = false; // async_write() 一定会将完整的 MsgNode 发送完成才回调 handle_write()
    }
}