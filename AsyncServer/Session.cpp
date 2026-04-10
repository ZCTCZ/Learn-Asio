//
// Created by AdminCZ on 2026/4/4.
//

#include "Session.h"
#include "Server.h"
#include <iostream>

#include "MsgNode.h"
#include "RecvNode.h"
#include "SendNode.h"

Session::Session(boost::asio::io_context& ioc, Server* server_ptr)
: m_socket(ioc),// 根据传入的上下文构建 socket
  m_server_ptr(server_ptr),
  m_recv_head(std::make_shared<RecvNode>(HEAD_TOTAL_LEN)) //#RecvNode 的 #m_msg_id 成员使用默认值，无意义。
{
    const auto uuid = boost::uuids::random_generator()();
    m_uuid = boost::uuids::to_string(uuid);
}

void Session::start()
{
    boost::asio::async_read(m_socket, boost::asio::buffer(m_recv_head->GetData(), HEAD_TOTAL_LEN),
        std::bind(&Session::handle_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

/**
* @return boost::asio::ip::tcp::socket 不支持拷贝，返回引用
*/
boost::asio::ip::tcp::socket& Session::Socket()
{
    return m_socket;
}

const std::string& Session::GetUUID() const
{
    return m_uuid;
}

void Session::Send(const char *msg, const MSG_LEN_TYPE len, const MSG_ID_TYPE id)
{
    {
        std::lock_guard<std::mutex> lock(m_send_que_mtx);

        /// 当发送队列已满，直接抛弃当前待发送数据
        if (m_MsgNode_que.size() >= MAX_QUE_LEN)
        {
            std::cerr << "Message Queue Is Full!" << std::endl;
            return;
        }

        m_MsgNode_que.push(std::make_shared<SendNode>(msg, len, id));

        if (m_pending) //上次还有 MsgNode 未发送完。将这次待发送的 MsgNode 添加到发送队列里即可
        {
            return ;
        }

        auto front_node = m_MsgNode_que.front();
        boost::asio::async_write(m_socket, boost::asio::buffer(front_node->GetData(), front_node->Get_Total_Len()),
            std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
        m_pending.store(true);
    }
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
            boost::asio::async_write(m_socket, boost::asio::buffer(node->GetData(), node->Get_Total_Len()),
                std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
            return;
        }

        m_pending = false; // async_write() 一定会将完整的 MsgNode 发送完成才回调 handle_write()
    }
}

/// 读取消息首部之后回调该函数
void Session::handle_read_head(const boost::system::error_code &ec, size_t transfer_bytes)
{
    if (ec)
    {
        if (ec == boost::asio::error::eof)
        {
            std::cerr << m_socket.remote_endpoint().address().to_string() << " disconnected!" << std::endl;
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

    /// 读取到的报文首部长度不等于 #HEAD_TOTAL_LEN。这种情况理论上来说不可能发生
    if (transfer_bytes != HEAD_TOTAL_LEN)
    {
        std::cerr << "Occurred Unknown Error" << std::endl;
        m_server_ptr->clear_session(m_uuid);
        return;
    }

    /// 解析出消息 id
    MSG_ID_TYPE msg_id = 0;
    memcpy(&msg_id, m_recv_head->GetData(), HEAD_ID_LEN);
    msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);

    /// 解析出消息长度
    MSG_LEN_TYPE data_len = 0;
    memcpy(&data_len, m_recv_head->GetData() + HEAD_ID_LEN, HEAD_DATA_LEN);
    data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);

    if (data_len > MAX_LENGTH)
    {
        std::cerr << "Invalid Data Length From " << m_socket.remote_endpoint().address().to_string() << std::endl;
        m_server_ptr->clear_session(m_uuid);
        return;
    }

    m_recv_body = std::make_shared<RecvNode>(data_len, msg_id);
    boost::asio::async_read(m_socket, boost::asio::buffer(m_recv_body->GetData(), data_len),
        std::bind(&Session::handle_read_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

/// 读取消息体之后回调该函数
void Session::handle_read_body(const boost::system::error_code &ec, size_t transfer_bytes)
{
    if (ec)
    {
        if (ec == boost::asio::error::eof)
        {
            std::cerr << m_socket.remote_endpoint().address().to_string() << " disconnected" << std::endl;
            m_server_ptr->clear_session(m_uuid);
            return;
        }

        std::cerr << "Occurred Error When Read"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        return;
    }

    if (transfer_bytes != m_recv_body->Get_Total_Len())
    {
        std::cerr << "Occurred UnKnown Error" << std::endl;
        m_server_ptr->clear_session(m_uuid);
        return;
    }

    std::cout << "Receive From " << m_socket.remote_endpoint().address().to_string()
                << "[ID:" << m_recv_body->Get_Msg_Id() << "] " << m_recv_body->GetData()<< std::endl;

    Send(m_recv_body->GetData(), m_recv_body->Get_Total_Len(), m_recv_body->Get_Msg_Id());
    m_recv_head->Clear();

    /// 睡眠 500 毫秒，使得服务器的tcp内核缓冲区的数据形成堆积，以此测试切包效果
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /// 开始新的消息报头接收
    boost::asio::async_read(m_socket, boost::asio::buffer(m_recv_head->GetData(), HEAD_TOTAL_LEN),
        std::bind(&Session::handle_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}