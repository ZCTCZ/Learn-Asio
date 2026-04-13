//
// Created by AdminCZ on 2026/4/11.
//

#include "LogicSystem.h"

#include <iostream>

#include "RecvNode.h"
#include "Session.h"

LogicSystem::LogicSystem()
{
    RegisterFuncCallBack();
    m_thread = std::thread(&LogicSystem::MainLogic, this); // 开启逻辑线程
}

LogicSystem::~LogicSystem()
{
    m_stop.store(true);
    m_cond_v.notify_one();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void LogicSystem::MainLogic()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cond_v.wait(lock);
        HandleQue();
        if (m_stop)
        {
            break;
        }
    }
}

void LogicSystem::HandleQue()
{
    while (!m_handle_que.empty())
    {
        auto front = m_handle_que.front();
        m_handle_que.pop();

        if (auto it = m_func_callbacks.find(front.m_id); it != m_func_callbacks.end())
        {
            it->second(front.m_session_ptr, front.m_id, front.m_data);
        }
    }
}

/// 给每种消息类型注册对应的处理函数
void LogicSystem::RegisterFuncCallBack()
{
    /// 注册 #MAG_HELLO_WORLD 消息的处理函数
    m_func_callbacks.insert({
        MSG_TYPE::MAG_HELLO_WORLD, [this](std::shared_ptr<Session> session, MSG_TYPE msg_id, const std::string& str)
        {
            HandleHelloWorld(session, msg_id, str);
        }
    });
}

void LogicSystem::PostMsgToQue(std::shared_ptr<Session> session, std::shared_ptr<RecvNode> recv_node)
{
    m_msg_que.emplace(session, recv_node);
    auto now = std::chrono::steady_clock::now();

    bool should_swap = m_msg_que.size() == MAX_MSG_QUE_CAPACITY ||
        (now - m_last_swap_time) > MAX_DELAY_MS;

    if (should_swap)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        /// 将备用队列空间和当前消息的空间进行交换
        /// 同时降低锁的粒度
        m_msg_que.swap(m_handle_que);

        m_last_swap_time = now;
        m_cond_v.notify_one(); // 逻辑线程可能被挂起，需要唤醒它
    }
}

void LogicSystem::HandleHelloWorld(std::shared_ptr<Session> session, MSG_TYPE msg_id, const std::string& str)
{
    std::cout << "Receive Message Id Is :" << static_cast<MSG_ID_TYPE>(msg_id) << std::endl;
    std::cout << "Receive From " << session->Socket().remote_endpoint().address().to_string()
        << "[ID:" << static_cast<MSG_ID_TYPE>(msg_id) << "] " <<
        "[Msg:" << str << "]" << std::endl;
    session->Send(str, msg_id);
}
