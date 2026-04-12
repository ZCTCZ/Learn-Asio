//
// Created by AdminCZ on 2026/4/11.
//

#ifndef ASYNCSERVER_LOGICSYSTEM_H
#define ASYNCSERVER_LOGICSYSTEM_H
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include "Singleton.h"
#include "const.h"
#include "RecvNode.h"

class Session;
class RecvNode;
class LogicSystem;

class LogicNode
{
    friend class LogicSystem;

public:
    LogicNode(std::shared_ptr<Session> session_ptr, std::shared_ptr<RecvNode> RecvNode_Ptr)
        : m_session_ptr{std::move(session_ptr)},
          m_id(RecvNode_Ptr->Get_Msg_Id()),
          m_data(std::string(RecvNode_Ptr->GetData(), RecvNode_Ptr->Get_Total_Len()))
    {
    }

private:
    std::shared_ptr<Session> m_session_ptr;
    MSG_TYPE m_id;
    std::string m_data;
};

/// 逻辑线程类，继承自单例模板类
class LogicSystem : public Singleton<LogicSystem>
{
    using FUNCCALLBACK = std::function<void(std::shared_ptr<Session>, MSG_TYPE, const std::string&)>;

public:
    friend Singleton; // 将基类 #Singleton 设置为友元类，因为基类需要调用 #LogicSystem 的构造函数
    ~LogicSystem() override;
    /// 提供给 #Session 类使用的接口，用于就将待处理的消息添加到消息处理队列
    void PostMsgToQue(std::shared_ptr<Session>, std::shared_ptr<RecvNode>);

private:
    LogicSystem();
    void RegisterFuncCallBack(); // 将 #FUNCCALLBACK 类型的函数添加到 #m_func_callbacks 里
    void MainLogic(); // 线程运行的主函数，负责逻辑处理
    void HandleHelloWorld(std::shared_ptr<Session>, MSG_TYPE, const std::string&); // 处理消息id为 #MAG_HELLO_WORLD 的消息
    std::thread m_thread; // 逻辑线程
    std::queue<LogicNode> m_msg_que; // 消息队列，存放待处理的消息
    std::mutex m_mtx;
    std::condition_variable m_cond_v;
    std::atomic_bool m_stop{false}; // 控制逻辑线程是否应该退出
    std::unordered_map<MSG_TYPE, FUNCCALLBACK> m_func_callbacks; // 记录每种消息类型所对应的处理函数
};


#endif //ASYNCSERVER_LOGICSYSTEM_H
