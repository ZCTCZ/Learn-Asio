//
// Created by AdminCZ on 2026/4/4.
//

#include "Session.h"
#include "Server.h"
#include <iostream>


void Session::start()
{
    m_socket.async_read_some(boost::asio::buffer(m_data, Session::BUF_SIZE),
        std::bind(&Session::handle_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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

    /// 调用 async_write() 函数，将对端发送过来的数据原封不动地转发回去
    /// async_write() 保证一定将用户缓冲区里的数据全部发送到内核的写缓冲区里
    boost::asio::async_write(m_socket, boost::asio::buffer(m_data, transfer_bytes),
        std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));

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

    memset(m_data, '\0', Session::BUF_SIZE);

    /// 调用 async_read_some() 函数，继续读取对端发送过来的数据
    m_socket.async_read_some(boost::asio::buffer(m_data, Session::BUF_SIZE),
        std::bind(&Session::handle_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}
