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

            /// 等待条件：队列达到容量上限、超时、或收到停止信号
            m_cond_v.wait_for(lock, MAX_DELAY_MS, [this]()
            {
                return m_msg_que.size() >= MAX_MSG_QUE_CAPACITY || m_stop.load();
            });

            /// 在锁内完成交换，此时 m_handle_que 一定是空的（上轮已处理完）
            if (!m_msg_que.empty())
            {
                m_msg_que.swap(m_handle_que);
            }
        }
        /// 锁外处理 m_handle_que —— 这就是双缓冲的意义：I/O 线程继续往 m_msg_que 里 push，互不干扰
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
    bool should_notify = false;
    {
        /// 可能有多个线程向 #m_msg_que 添加数据，所以需要加锁
        std::lock_guard<std::mutex> lock(m_mtx);
        m_msg_que.emplace(session, recv_node);
        should_notify = m_msg_que.size() >= MAX_MSG_QUE_CAPACITY;
    }
    
    /// 队列满了就立即唤醒逻辑线程，不用等超时
    if (should_notify)
    {
        m_cond_v.notify_one();
    }
}

void LogicSystem::HandleHelloWorld(std::shared_ptr<Session> session, MSG_TYPE msg_id, const std::string& str)
{
    boost::system::error_code ec;
    auto ep = session->Socket().remote_endpoint(ec);
    if (ec) // 对端已经关闭，不需要在处理它发送过来的数据
    {
        std::cerr << "Session already disconnected, skip handling" << std::endl;
        return;
    }

    std::cout << "Receive Message Id Is :" << static_cast<MSG_ID_TYPE>(msg_id) << std::endl;
    std::cout << "Receive From " << ep.address().to_string()
        << "[ID:" << static_cast<MSG_ID_TYPE>(msg_id) << "] " <<
        "[Msg:" << str << "]" << std::endl;
    session->Send(str, msg_id);
}
