//
// Created by AdminCZ on 2026/4/10.
//

#include "RecvNode.h"

RecvNode::RecvNode(const MSG_LEN_TYPE len, const MSG_TYPE id)
    : MsgNode(len),
      m_msg_id(id)
{
}

RecvNode::~RecvNode() = default;

MSG_TYPE RecvNode::Get_Msg_Id() const
{
    return m_msg_id;
}
