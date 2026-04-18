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
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_cond_v.wait_for(lock, MAX_DELAY_MS, [this]()
            {
                return m_msg_que.size() >= MAX_MSG_QUE_CAPACITY || m_stop;
            });

            if (!m_msg_que.empty())
            {
                std::swap(m_msg_que, m_handle_que);
            }
        }

        /// 逻辑线程处理数据期间，不需要持有锁
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

void LogicSystem::PostMsgToQue(const std::shared_ptr<Session>& session, const std::shared_ptr<RecvNode>& recv_node)
{
    bool should_notify = false;
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_msg_que.emplace(session, recv_node);
        should_notify = m_msg_que.size() >= MAX_MSG_QUE_CAPACITY;
    }
 /// 在锁外面 notify,可以减少无用的上下文切换
    if (should_notify)
    {
        m_cond_v.notify_one();
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
