//
// Created by AdminCZ on 2026/4/28.
//

#include "LogicSystem.h"
#include "RecvNode.h"
#include "Session.h"

#include <mutex>
#include <utility>
#include <nlohmann/json.hpp>
#include <print>

LogicNode::LogicNode(std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> recv_node_ptr, MSG_TYPE msg_id) :
    m_session_ptr(std::move(session_ptr)), m_recv_node_ptr(std::move(recv_node_ptr)), m_msg_id(msg_id)
{
}

LogicSystem::LogicSystem() = default;

void LogicSystem::Run()
{
    RegisterFunc();
    m_worker = std::thread(&LogicSystem::Loop, this);
}

LogicSystem& LogicSystem::GetInstance()
{
    static LogicSystem instance;
    static std::once_flag flag;

    std::call_once(flag, []() {
        instance.Run();
    });

    return instance;
}

void LogicSystem::Loop()
{
    while (!m_stop.load()) {
        {
            std::unique_lock<std::mutex> lock(m_msg_que_mtx);
            m_cond_val.wait_for(lock, std::chrono::milliseconds(LOGIC_WAIT_TIME), [this]() {
                return !m_msg_que.empty() || m_stop;
            });

            if (!m_msg_que.empty()) {
                std::swap(m_msg_que, m_handle_que);
            }
        }

        Handle_HandleQue();
    }
}

void LogicSystem::RegisterFunc()
{
    /// 注册处理 MSG_HELLO_WORLD 的函数
    m_func_map.insert({MSG_TYPE::MSG_HELLO_WORLD,
                       [this](std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> recv_node_ptr,
                              MSG_TYPE msg_id) {
                           this->HANDLE_MSG_HELLO_WORLD(std::move(session_ptr), std::move(recv_node_ptr), msg_id);
                       }});
}

void LogicSystem::PostToMsgQue(std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> recv_node_ptr)
{
    bool should_notify{false};
    {
        std::lock_guard<std::mutex> lock(m_msg_que_mtx);
        auto msg_id = recv_node_ptr->GetMsgId();
        m_msg_que.emplace(std::move(session_ptr), std::move(recv_node_ptr), msg_id);

        should_notify = m_msg_que.size() >= LOGIC_MAX_MSG_QUE_LEN;
    }

    if (should_notify) {
        m_cond_val.notify_one();
    }
}

LogicSystem::~LogicSystem()
{
    Stop();
}

void LogicSystem::Stop()
{
    m_stop.exchange(true);
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void LogicSystem::Handle_HandleQue()
{
    while (!m_handle_que.empty()) {
        auto front = std::move(m_handle_que.front()); // 调用移动构造
        m_handle_que.pop();
        auto msg_id = front.m_msg_id;

        auto it = m_func_map.find(msg_id);
        if (it != m_func_map.end()) {
            it->second(std::move(front.m_session_ptr), std::move(front.m_recv_node_ptr), msg_id);
        }
    }
}

void LogicSystem::HANDLE_MSG_HELLO_WORLD(std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> recv_node_ptr,
                                         MSG_TYPE msg_id)
{
    if (session_ptr->IsClose()) {
        std::println("Session already disconnected");
        return;
    }

    auto json = nlohmann::json::parse(recv_node_ptr->GetData());
    auto msg = json["msg"].get<std::string>();
    // std::println("Recv From {}, MsgId = {}, Msg = {}", session_ptr->Get_Remote_Address(),
    //              static_cast<MSG_ID_TYPE>(msg_id), msg);

    nlohmann::json send_json;
    send_json["id"] = msg_id;
    send_json["msg"] = "server received message:" + msg;
    auto send_str = send_json.dump();
    session_ptr->Send(send_str, msg_id);
}



