//
// Created by AdminCZ on 2026/4/24.
//

#ifndef COROUTINE_ASYNCSERVER_IOCONTEXTPOOL_H
#define COROUTINE_ASYNCSERVER_IOCONTEXTPOOL_H
#include <boost/asio.hpp>

class IOContextPool {
public:
    explicit IOContextPool(std::size_t nums = std::thread::hardware_concurrency());
    IOContextPool(const IOContextPool&) = delete;
    IOContextPool& operator=(const IOContextPool&) = delete;
    ~IOContextPool();

    void Run();
    void Stop() noexcept;
    boost::asio::io_context& GetIoContext() noexcept;

private:
    using m_io_context_ptr = std::shared_ptr<boost::asio::io_context>;
    using m_io_context_work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    std::vector<m_io_context_ptr> m_io_contexts;
    std::vector<m_io_context_work> m_io_contexts_work_guards;
    std::vector<std::thread> m_threads;
    std::atomic<std::size_t> m_ioc_next{0}; // 使用原子变量，确保线程安全
};


#endif //COROUTINE_ASYNCSERVER_IOCONTEXTPOOL_H
