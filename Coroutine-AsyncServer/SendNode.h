//
// Created by AdminCZ on 2026/4/23.
//

#ifndef COROUTINE_ASYNCSERVER_SENDNODE_H
#define COROUTINE_ASYNCSERVER_SENDNODE_H
#include "MsgNode.h"
#include "constant.h"


class SendNode : public MsgNode {
public:
    explicit SendNode(const char* msg, unsigned int msg_len, MSG_TYPE id);
    ~SendNode() override = default;
    [[nodiscard]] MSG_TYPE GetMsgId() const noexcept;

private:
    MSG_TYPE m_id;
};


#endif //COROUTINE_ASYNCSERVER_SENDNODE_H
