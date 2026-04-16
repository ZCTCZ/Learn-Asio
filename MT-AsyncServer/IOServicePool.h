//
// Created by AdminCZ on 2026/4/15.
//

#ifndef ASYNCSERVER_IOSERVICEPOOL_H
#define ASYNCSERVER_IOSERVICEPOOL_H
#include <vector>
#include <boost/asio.hpp>

#include "Singleton.h"


class IOServicePool : public Singleton<IOServicePool>
{
    friend class Singleton;
    using io_worker_guard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

public:
    boost::asio::io_context& get_ioc();
    void stop();
    ~IOServicePool() override;

private:
    IOServicePool(size_t num = std::thread::hardware_concurrency());

    /**
     * io_context.run() 是 Asio 的核心事件循环。它的默认行为是：
     *  检查当前是否有挂起的异步操作（如 async_read、async_write、timer 等）。
     *  如果有，它会阻塞并等待这些操作完成并调用回调。
     *  如果没有异步操作了，或者所有的异步操作都已处理完毕，run() 会立即返回。
     */
    std::vector<boost::asio::io_context> m_vec_iocs;
    /**
     * 如果有了  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> 对象，
     * 即使此时没有异步操作挂载到 io_context上，run() 也不会返回。
     * 需要  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> 调用 reset() 函数，run() 才会返回。
     */
    std::vector<io_worker_guard> m_vec_guards;

    std::vector<std::thread> m_threads;
    size_t m_next; //轮询计数器
    std::mutex m_mtx; // #m_vec_guards 是临界资源，在多线程环境下使用需加锁
};


#endif //ASYNCSERVER_IOSERVICEPOOL_H
