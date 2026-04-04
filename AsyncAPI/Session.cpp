//
// Created by AdminCZ on 2026/4/3.
//

#include "Header/Session.h"

#include <iostream>

/**
 * @brief async_write_some 结束后，执行的回调函数
 * 每次回调只处理当前完成的这一部分，然后通过发起新的异步操作来继续
 * @param ec 记录 async_write_some 执行的结果
 * @param transfer_bytes async_write_some 发送的字节数
 */
void Session::WriteSomeCallBackErr(boost::system::error_code ec, size_t transfer_bytes)
{
    if (ec)
    {
        std::cerr << "WriteSome Data Occur Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        m_send_pending = false;
        return ;
    }

    auto node = m_send_que.front();
    node->m_cur_len += transfer_bytes;

    if (node->m_cur_len < node->m_total_len)
    {
        m_socket_ptr->async_write_some(boost::asio::buffer(node->m_msg + node->m_cur_len, node->m_total_len - node->m_cur_len),
            std::bind(&Session::WriteSomeCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
        return ;
    }

    m_send_que.pop(); // 将已完成发送的 MsgNode 移除队列
    if (!m_send_que.empty())
    {
        node = m_send_que.front();
        m_socket_ptr->async_write_some(boost::asio::buffer(node->m_msg, node->m_total_len),
            std::bind(&Session::WriteSomeCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
        return ;
    }

    m_send_pending = false;
}


/**
 * @brief 调用异步函数 async_write_some 发送数据
 * @param buf 待发送的数据串
 */
void Session::WriteSomeToSocketErr(const std::string &buf)
{
    auto node = std::make_shared<MsgNode>(buf.c_str(), buf.size());
    m_send_que.push(node);
    if (m_send_pending)
    {
        return; // 有数据还未发送完，将发送任务（node）添加到发送队列末尾
    }

    m_socket_ptr->async_write_some(boost::asio::buffer(node->m_msg, node->m_total_len),
        std::bind(&Session::WriteSomeCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
    m_send_pending = true; // 有数据正在发送
}

/**
 * @brief async_send() 函数执行结束后的回调函数。
 * async_send() 可能只发送部分数据，需要检查 transfer_bytes 并继续发送剩余数据
 * @param ec 记录 async_send() 执行的结果
 * @param transfer_bytes async_send 实际发送的字节数（可能小于缓冲区大小）
 */
void Session::SendCallBackErr(boost::system::error_code ec, size_t transfer_bytes)
{
    if (ec)
    {
        std::cerr << "Send Data Occur Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        m_send_pending = false;
        return ;
    }

    auto node = m_send_que.front();
    node->m_cur_len += transfer_bytes;

    if (node->m_cur_len < node->m_total_len)
    {
        m_socket_ptr->async_send(boost::asio::buffer(node->m_msg + node->m_cur_len, node->m_total_len - node->m_cur_len),
            std::bind(&Session::SendCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }

    m_send_que.pop();
    if (!m_send_que.empty())
    {
        node = m_send_que.front();
        m_socket_ptr->async_send(boost::asio::buffer(node->m_msg, node->m_total_len),
            std::bind(&Session::SendCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }

    m_send_pending = false;
}

/**
 * @brief 将待发送的数据添加到发送队列并执行发送
 * @param buf 待发送的数据
 */
void Session::SendToSocketErr(const std::string &buf)
{
    auto node = std::make_shared<MsgNode>(buf.c_str(), buf.size());
    m_send_que.push(node);

    if (m_send_pending)
    {
        return ;// 有数据还未发送完，将发送任务（node）添加到发送队列末尾
    }

    // 调用异步发送函数 async_send
    m_socket_ptr->async_send(boost::asio::buffer(node->m_msg, node->m_total_len),
        std::bind(&Session::SendCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
    m_send_pending = true; // 记录有数据正在发送
}

/**
 * @brief async_write 完成后的回调函数
 * async_write 会自动处理部分发送，保证所有数据发送完成（或出错）
 * @param ec 错误码，如果成功则为 boost::system::error_code()
 * @param transfer_bytes 实际传输的字节数（通常等于节点总长度）
 */
void Session::WriteCallBackErr(boost::system::error_code ec, size_t transfer_bytes)
{
    if (ec)
    {
        std::cerr << "Write Data Occur Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        m_send_pending = false;
        return ;
    }

    m_send_que.pop();

    if (!m_send_que.empty())
    {
        auto node = m_send_que.front();
        boost::asio::async_write(*m_socket_ptr, boost::asio::buffer(node->m_msg, node->m_total_len),
            std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
        return ;
    }

    m_send_pending = false;
}

/**
 * @brief 使用 async_write 将数据发送到 socket
 * async_write 会自动循环直到所有数据发送完成（或发生错误）
 * @param buf 待发送的数据
 */
void Session::WriteToSocketErr(const std::string &buf)
{
    auto node = std::make_shared<MsgNode>(buf.c_str(), buf.size());
    m_send_que.push(node);
    if (m_send_pending)
    {
        return ;
    }

    boost::asio::async_write(*m_socket_ptr, boost::asio::buffer(node->m_msg, node->m_total_len),
        std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
    m_send_pending = true;
}

void Session::ReadSomeCallBackErr(boost::system::error_code ec, size_t transfer_bytes)
{
    if (ec)
    {
        std::cerr << "ReadSome Data Occur Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        m_read_pending = false;
        m_read_node = nullptr;
        return ;
    }
    m_read_node->m_cur_len += transfer_bytes;
    if (m_read_node->m_cur_len < m_read_node->m_total_len)
    {
        m_socket_ptr->async_read_some(boost::asio::buffer(m_read_node->m_msg + m_read_node->m_cur_len, m_read_node->m_total_len - m_read_node->m_cur_len),
            std::bind(&Session::ReadSomeCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }

    m_read_node = nullptr;
    m_read_pending = false;
}

void Session::ReadSomeFromSocketErr()
{
    if (m_read_pending) // 如果有异步读操作在执行，则跳过
    {
        return;
    }

    m_read_node = std::make_shared<MsgNode>(MsgNode::MAXSIZE);
    m_socket_ptr->async_read_some(boost::asio::buffer(m_read_node->m_msg, m_read_node->m_total_len),
        std::bind(&Session::ReadSomeCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
    m_read_pending = true;
}
void Session::ReceiveCallbackErr(boost::system::error_code ec, size_t transfer_bytes)
{
    if (ec)
    {
        std::cerr << "Receive Data Occur Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        m_read_pending = false;
        m_read_node = nullptr;
        return ;
    }
    m_read_node->m_cur_len += transfer_bytes;
    if (m_read_node->m_cur_len < m_read_node->m_total_len)
    {
        m_socket_ptr->async_receive(boost::asio::buffer(m_read_node->m_msg + m_read_node->m_cur_len, m_read_node->m_total_len - m_read_node->m_cur_len),
            std::bind(&Session::ReceiveCallbackErr, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }

    m_read_node = nullptr;
    m_read_pending = false;
}
void Session::ReceiveFromSocketErr()
{
    if (m_read_pending)
    {
        return;
    }

    m_read_node = std::make_shared<MsgNode>(MsgNode::MAXSIZE);
    m_socket_ptr->async_receive(boost::asio::buffer(m_read_node->m_msg, m_read_node->m_total_len),
       std::bind(&Session::ReceiveCallbackErr, this, std::placeholders::_1, std::placeholders::_2));
    m_read_pending = true;
}
void Session::ReadCallBackErr(boost::system::error_code ec, size_t transfer_bytes)
{
    if (ec)
    {
        std::cerr << "Read Data Occur Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        m_read_pending = false;
        m_read_node = nullptr;
        return ;
    }
    m_read_node->m_cur_len += transfer_bytes;
    m_read_node = nullptr;
    m_read_pending = false;
}
void Session::ReadFromSocketErr()
{
    if (m_read_pending)
    {
        return;
    }

    m_read_node = std::make_shared<MsgNode>(MsgNode::MAXSIZE);
    boost::asio::async_read(*m_socket_ptr, boost::asio::buffer(m_read_node->m_msg, m_read_node->m_total_len),
        std::bind(&Session::ReadCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
    m_read_pending = true;
}