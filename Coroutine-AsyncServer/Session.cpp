//
// Created by AdminCZ on 2026/4/24.
//

#include "Session.h"
#include "RecvNode.h"
#include "constant.h"
#include "SendNode.h"
#include "Server.h"
#include "LogicSystem.h"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/generators.hpp>
#include <print>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/read.hpp>

Session::Session(boost::asio::io_context& io_context, Server* server_ptr) :
    m_socket(io_context), m_server_ptr(server_ptr), m_recv_head_node(new RecvNode(HEAD_TOTAL_LEN))
{
    m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
}

Session::~Session()
{
    Close();
}

void Session::Run()
{
    auto shared_this = shared_from_this();
    constexpr auto
        use_nothrow_awaitable =
            boost::asio::as_tuple(boost::asio::use_awaitable);

    /// 给每个连接分配一个协程
    boost::asio::co_spawn(m_socket.get_executor(),
                          [shared_this, &use_nothrow_awaitable]()-> boost::asio::awaitable<void> {
                              while (!shared_this->m_closed.load()) {
                                  /// 首先接收数据报首部
                                  auto [head_ec, head_transfer_num] = co_await boost::asio::async_read(
                                      shared_this->GetSocket(),
                                      boost::asio::buffer(
                                          shared_this->m_recv_head_node->GetData(),
                                          HEAD_TOTAL_LEN),
                                      use_nothrow_awaitable);

                                  if (head_ec) {
                                      if (head_ec == boost::asio::error::eof) {
                                          std::println("{} close", shared_this->Get_Remote_Address());
                                      }

                                      shared_this->Close();
                                      shared_this->m_server_ptr->Clear_Session(shared_this->GetUUID());
                                      co_return ;
                                  }

                                  /// 解析首部数据
                                  MSG_ID_TYPE msg_id = 0;
                                  memcpy(&msg_id, shared_this->m_recv_head_node->GetData(), HEAD_ID_LEN);
                                  msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);

                                  uint16_t data_len = 0;
                                  memcpy(&data_len, shared_this->m_recv_head_node->GetData() + HEAD_ID_LEN,
                                         HEAD_DATA_LEN);
                                  data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);
                                  if (data_len > MSG_MAX_LEN) {
                                      std::println("{} send data too long!", shared_this->Get_Remote_Address());
                                      shared_this->Close();
                                      shared_this->m_server_ptr->Clear_Session(shared_this->GetUUID());
                                      co_return ;
                                  }

                                  /// 接着接收数据报的报体
                                  shared_this->m_recv_body_node = std::make_shared<RecvNode>(
                                      data_len, static_cast<MSG_TYPE>(msg_id));

                                  auto [body_ec, body_transfer_num] = co_await boost::asio::async_read(
                                      shared_this->GetSocket(),
                                      boost::asio::buffer(shared_this->m_recv_body_node->GetData(), data_len),
                                      use_nothrow_awaitable);

                                  if (body_ec) {
                                      if (body_ec == boost::asio::error::eof) {
                                          std::println("{} close", shared_this->Get_Remote_Address());
                                      }
                                      shared_this->Close();
                                      shared_this->m_server_ptr->Clear_Session(shared_this->GetUUID());
                                      co_return ;
                                  }

                                  /// 将接收到的整个数据报报体传给逻辑线程处理
                                  LogicSystem::GetInstance().PostToMsgQue(shared_this, shared_this->m_recv_body_node);
                              }
                          }, boost::asio::detached);
}

void Session::Send(const char* msg, std::size_t msg_len, MSG_TYPE id)
{
    std::lock_guard<std::mutex> lock(m_send_que_mtx);

    if (m_send_que.size() >= MAX_SEND_QUE_LEN) {
        std::println("Message Queue Is Full! Max Capacity Is {}", MAX_SEND_QUE_LEN);
        return;
    }

    m_send_que.emplace(std::make_shared<SendNode>(msg, msg_len, id));
    if (m_pending.load()) {
        return;
    }
    m_pending.exchange(true);

    auto front_node = m_send_que.front();

    boost::asio::async_write(m_socket, boost::asio::buffer(front_node->GetData(), front_node->GetTotalLen()),
                             [shared_this = shared_from_this()](const boost::system::error_code& ec, std::size_t nums) {
                                 shared_this->HandleWrite(ec, nums);
                             });
}

void Session::Send(const std::string& msg, MSG_TYPE id)
{
    Send(msg.data(), msg.size(), id);
}

const boost::asio::ip::tcp::socket& Session::GetSocket() const noexcept
{
    return m_socket;
}

void Session::HandleWrite(const boost::system::error_code& ec, std::size_t)
{
    std::lock_guard<std::mutex> lock(m_send_que_mtx);
    m_send_que.pop();

    if (ec) {
        if (ec == boost::asio::error::eof) {
            std::println("{} close", Get_Remote_Address());
        }
        else {
            std::println("send message to {} error! error code = {}, error message = {}",
                         Get_Remote_Address(), ec.value(), ec.message());
        }
        Close();
        m_server_ptr->Clear_Session(m_uuid);
        return;
    }

    if (!m_send_que.empty()) {
        auto front_node = m_send_que.front();
        boost::asio::async_write(m_socket, boost::asio::buffer(front_node->GetData(), front_node->GetTotalLen()),
                                 [shared_this = shared_from_this()](const boost::system::error_code& ec,
                                                                    std::size_t nums) {
                                     shared_this->HandleWrite(ec, nums);
                                 });
        return;
    }

    m_pending.exchange(false);
}

bool Session::IsClose() const noexcept
{
    return m_closed.load();
}

boost::asio::ip::tcp::socket& Session::GetSocket() noexcept
{
    return m_socket;
}

const std::string& Session::GetUUID() const noexcept
{
    return m_uuid;
}

const std::string& Session::Get_Remote_Address() const noexcept
{
    return m_remote_address;
}

void Session::Set_Remote_Address(std::string remote_address)
{
    m_remote_address = std::move(remote_address);
}

void Session::Close()
{
    if (!m_closed.exchange(true)) {
        boost::system::error_code ec;
        m_socket.close(ec);
    }
}


