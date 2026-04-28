//
// Created by AdminCZ on 2026/4/23.
//

#ifndef COROUTINE_ASYNCSERVER_RECVNODE_H
#define COROUTINE_ASYNCSERVER_RECVNODE_H
#include "MsgNode.h"
#include "constant.h"

class RecvNode : public MsgNode {
public:
    explicit RecvNode(unsigned int len, MSG_TYPE id = MSG_TYPE::MSG_INVALID_ID);
    ~RecvNode() override = default;
    [[nodiscard]] MSG_TYPE GetMsgId() const noexcept;

private:
    MSG_TYPE m_id;
};


#endif //COROUTINE_ASYNCSERVER_RECVNODE_H
