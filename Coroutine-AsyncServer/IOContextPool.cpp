//
// Created by AdminCZ on 2026/4/24.
//

#include "IOContextPool.h"

IOContextPool::IOContextPool(std::size_t nums)
{
    if (nums == 0) {
        throw std::runtime_error("IOContextPool size is 0");
    }

    for (std::size_t i = 0; i < nums; ++i) {
        m_io_contexts.emplace_back(new boost::asio::io_context());
        m_io_contexts_work_guards.emplace_back(boost::asio::make_work_guard(*m_io_contexts.back()));
    }
}

boost::asio::io_context& IOContextPool::GetIoContext() noexcept
{
    return *m_io_contexts[m_ioc_next.fetch_add(1) % m_io_contexts.size()];
}

void IOContextPool::Run()
{
    for (auto& m_io_context : m_io_contexts) {
        m_threads.emplace_back([m_io_context]() {
            m_io_context->run();
        });
    }
}

void IOContextPool::Stop() noexcept
{
    for (auto& m_io_contexts_work_guard : m_io_contexts_work_guards) {
        m_io_contexts_work_guard.reset();
    }

    for (auto& m_thread : m_threads) {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

IOContextPool::~IOContextPool()
{
    Stop();
}
