//
// Created by AdminCZ on 2026/4/18.
//

#include "IOThreadPool.h"

IOThreadPool::IOThreadPool(size_t nums)
    :m_ioc(boost::asio::io_context()),
    m_work_guard(boost::asio::make_work_guard(m_ioc))
{
    for (size_t i = 0; i < nums; ++i)
    {
        m_vec_threads.emplace_back([this]()
        {
            m_ioc.run();
        });
    }
}

boost::asio::io_context& IOThreadPool::GetContext()
{
    return m_ioc;
}

void IOThreadPool::stop()
{
    m_work_guard.reset();
    m_ioc.stop();

    for (auto &thread : m_vec_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

IOThreadPool::~IOThreadPool()
{
    stop();
}
