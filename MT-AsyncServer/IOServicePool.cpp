//
// Created by AdminCZ on 2026/4/15.
//

#include "IOServicePool.h"

IOServicePool::IOServicePool(size_t num)
    : m_vec_iocs(num),
      m_next(0)
{
    m_vec_guards.reserve(num);
    m_threads.reserve(num);

    for (size_t i = 0; i < num; ++i)
    {
        m_vec_guards.push_back(boost::asio::make_work_guard(m_vec_iocs[i]));
    }

    for (size_t i = 0; i < num; ++i)
    {
        m_threads.emplace_back([this, i]()
        {
            m_vec_iocs[i].run();
        });
    }
}

boost::asio::io_context& IOServicePool::get_ioc()
{
    std::lock_guard<std::mutex> lock(m_mtx);
    auto& ret = m_vec_iocs[m_next];
    m_next = m_next + 1 == m_vec_iocs.size() ? 0 : m_next + 1;
    return ret;
}

void IOServicePool::stop()
{
    for (auto& e : m_vec_guards)
    {
        e.reset(); // 减少引用计数，使得子线程里 run() 函数退出，从而结束子线程
    }

    for (auto& thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

IOServicePool::~IOServicePool()
{
    stop();
}


