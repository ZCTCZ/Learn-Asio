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

    int cur_copy_pos = 0; // 当前处理到 m_data 的哪个位置
    while (transfer_bytes > 0)
    {
        if (!m_head_parsed) // 报文头部还没有解析完成
        {
            int need_len = MsgNode::HEAD_LENGTH - m_recv_head->m_cur_len; // 构成一个完整的消息首部，需要的字节数
            int can_copy = std::min(need_len, static_cast<int>(transfer_bytes)); // 当前所能拷贝的最大字节数

            memcpy(m_recv_head->m_data + m_recv_head->m_cur_len, m_data + cur_copy_pos, can_copy);
            m_recv_head->m_cur_len += can_copy;
            cur_copy_pos += can_copy;
            transfer_bytes -= can_copy;

            if (m_recv_head->m_cur_len == MsgNode::HEAD_LENGTH) // 首部 已收集齐了
            {
                int data_len = 0; // 该条数据体的长度
                memcpy(&data_len, m_recv_head->m_data, MsgNode::HEAD_LENGTH);
                if (data_len > Session::MAX_LENGTH)
                {
                    std::cerr << "Invalid Data Length From " << m_socket.remote_endpoint().address().to_string() << std::endl;
                    m_server_ptr->clear_session(m_uuid); // 将该连接直接断开
                    return;
                }

                /// 生成记录消息体的结点
                m_recv_body = std::make_shared<MsgNode>(data_len);
                m_head_parsed = true;
            }
            else
            {
                break;
            }
        }

        /// 消息头部已经处理完成
        int need_len = m_recv_body->m_total_len - m_recv_body->m_cur_len;
        int can_copy = std::min(need_len, static_cast<int>(transfer_bytes));

        memcpy(m_recv_body->m_data + m_recv_body->m_cur_len, m_data + cur_copy_pos, can_copy);
        m_recv_body->m_cur_len += can_copy;
        cur_copy_pos += can_copy;
        transfer_bytes -= can_copy;

        if (m_recv_body->m_cur_len == m_recv_body->m_total_len) // 消息体收集齐了
        {
            m_recv_body->m_data[m_recv_body->m_total_len] = '\0';
            std::cout << "Receive From " << m_socket.remote_endpoint().address().to_string() << " ["
                << m_recv_body->m_data << "]" << std::endl;

            Send(m_recv_body->m_data, m_recv_body->m_total_len);

            /// 重置状态，准备接收下一条消息的首部
            m_head_parsed = false;
            m_recv_head->Clear();
        }
        else
        {
            break;
        }
    }

    // 统一在循环外边调用 async_read_some
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
            boost::asio::async_write(m_socket, boost::asio::buffer(node->m_data, node->m_total_len),
                std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
            return;
        }

        m_pending = false; // async_write() 一定会将完整的 MsgNode 发送完成才回调 handle_write()
    }
}