//
// Created by AdminCZ on 2026/4/24.
//

#include "Server.h"
#include "Session.h"
#include <print>

Server::Server(const std::string& ip_address, uint16_t port, std::size_t threads_nums) :
    m_io_context(boost::asio::io_context()),
    m_acceptor(m_io_context),
    m_io_context_pool(threads_nums),
    m_signals(m_io_context)
{
    Set_Await_Signal();

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(ip_address), port);
    m_acceptor.open(ep.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true)); // 设置端口服用
    m_acceptor.bind(ep);
    m_acceptor.listen();

    Start_Accept();
}

void Server::Start_Accept()
{
    /// new_session (里的 m_socket) 依赖 m_io_context_pool（里的 io_context）
    auto new_session = std::make_shared<Session>(m_io_context_pool.GetIoContext(), this);

    /// async_accept 的完成处理器捕获了 new_session，处理器挂在 m_acceptor 上
    /// 但 m_acceptor 析构后，处理器会被投递到 m_io_context 的队列，使 new_session 活到 m_io_context 析构
    /// 依赖链：m_io_context → new_session → pool 的 io_context
    /// 所以 m_io_context_pool 必须在 m_io_context 之后析构（声明在前面）
    m_acceptor.async_accept(new_session->GetSocket(),
                            [this, new_session](const boost::system::error_code& error) mutable {
                                Handle_Accept(error, std::move(new_session));
                            });
}

void Server::Handle_Accept(const boost::system::error_code& error, std::shared_ptr<Session> new_session)
{
    if (error) {
        std::println("Accept Connection Error! Error Code = {}, Error Message = {}", error.value(), error.message());
        return;
    }

    new_session->Set_Remote_Address(new_session->GetSocket().remote_endpoint().address().to_string());
    std::println("New Connection: {}", new_session->Get_Remote_Address());
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_session_map.insert({new_session->GetUUID(), new_session});
    }
    new_session->Run();

    Start_Accept();
}

void Server::Run()
{
    m_io_context_pool.Run();
    m_io_context.run();
}

void Server::Stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        for (auto& [uuid, session_ptr] : m_session_map) {
            session_ptr->Close();
        }
        m_session_map.clear();
    }

    m_io_context_pool.Stop();
    m_acceptor.close();
    m_io_context.stop();
}

Server::~Server()
{
    Stop();
}

void Server::Set_Await_Signal()
{
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
    m_signals.add(SIGABRT);

    m_signals.async_wait([this](const boost::system::error_code& error, int signal_num) {
        if (!error) {
            std::println("capture signal: {}\nserver close!", signal_num);
        }
        Stop();
    });
}

void Server::Clear_Session(const std::string& uuid)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    m_session_map.erase(uuid);
}
