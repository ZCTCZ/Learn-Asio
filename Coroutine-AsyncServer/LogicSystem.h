//
// Created by AdminCZ on 2026/4/28.
//

#ifndef COROUTINE_ASYNCSERVER_LOGICSYSTEM_H
#define COROUTINE_ASYNCSERVER_LOGICSYSTEM_H
#include <condition_variable>
#include <functional>

#include "constant.h"

#include <memory>
#include <queue>
#include <thread>
#include <unordered_map>

class RecvNode;
class Session;
class LogicSystem;

class LogicNode {
    friend class LogicSystem;
    LogicNode(LogicNode&&) = default;

public:
    LogicNode(std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> recv_node_ptr, MSG_TYPE msg_id);

private:
    std::shared_ptr<Session> m_session_ptr;
    std::shared_ptr<RecvNode> m_recv_node_ptr;
    MSG_TYPE m_msg_id;
};


class LogicSystem {
public:
    LogicSystem(const LogicSystem&) = delete;
    LogicSystem& operator=(const LogicSystem&) = delete;
    static LogicSystem& GetInstance();
    void PostToMsgQue(std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> recv_node_ptr);

private:
    using FUNCCALLBACK = std::function<void(std::shared_ptr<Session>, std::shared_ptr<RecvNode>, MSG_TYPE)>;
    void HANDLE_MSG_HELLO_WORLD(std::shared_ptr<Session>, std::shared_ptr<RecvNode>, MSG_TYPE);

    LogicSystem();
    void Run();
    void RegisterFunc();
    void Loop();
    void Handle_HandleQue();
    void Stop();
    ~LogicSystem();

    std::queue<LogicNode> m_msg_que; // I/O线程将待处理地数据送到 m_msg_que
    std::queue<LogicNode> m_handle_que; // 逻辑线程从 m_handle_que 里取出数据处理
    std::thread m_worker;
    std::mutex m_msg_que_mtx;
    std::condition_variable m_cond_val;
    std::atomic<bool> m_stop{false};
    std::unordered_map<MSG_TYPE, FUNCCALLBACK> m_func_map;
};


#endif //COROUTINE_ASYNCSERVER_LOGICSYSTEM_H
