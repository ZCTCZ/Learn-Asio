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
        m_cond_v.wait(lock, [this]()
        {
            return !m_msg_que.empty() || m_stop;
        });

        if (m_stop)
        {
            /// 清空队列里待处理的消息
            while (!m_msg_que.empty())
            {
                auto front = m_msg_que.front();
                m_msg_que.pop();

                if (auto it = m_func_callbacks.find(front.m_id); it != m_func_callbacks.end())
                {
                    it->second(front.m_session_ptr, front.m_id, front.m_data);
                }
            }
            lock.unlock();
            return; // 结束逻辑线程
        }

        /// 只取出队首元素，防止逻辑线程占用 #m_msg_que 时间过程，耽误主线程
        auto front = m_msg_que.front();
        m_msg_que.pop();
        lock.unlock();
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
    std::lock_guard<std::mutex> lock(m_mtx);
    m_msg_que.emplace(session, recv_node);
    if (m_msg_que.size() == 1)
    {
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
