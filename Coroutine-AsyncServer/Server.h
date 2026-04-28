//
// Created by AdminCZ on 2026/4/24.
//

#ifndef COROUTINE_ASYNCSERVER_SERVER_H
#define COROUTINE_ASYNCSERVER_SERVER_H

#include "IOContextPool.h"

class Session;

class Server {
public:
    explicit Server(const std::string& ip_address, uint16_t port,
                    std::size_t threads_nums = std::thread::hardware_concurrency());
    ~Server();
    void Run();
    void Stop();
    void Clear_Session(const std::string& uuid);

private:
    void Start_Accept();
    void Handle_Accept(const boost::system::error_code&, std::shared_ptr<Session>);
    void Set_Await_Signal(); // 设置需要捕获的信号

    boost::asio::io_context m_io_context; // 主事件循环，只负责监听新连接建立
    boost::asio::ip::tcp::acceptor m_acceptor;
    IOContextPool m_io_context_pool; // 从事件循环池
    boost::asio::signal_set m_signals; // 需要捕获的信号
    std::unordered_map<std::string, std::shared_ptr<Session>> m_session_map;
    std::mutex m_mtx; // 信号量，用于保护共享资源 @ref m_session_map
};


#endif //COROUTINE_ASYNCSERVER_SERVER_H
