//
// Created by AdminCZ on 2026/4/10.
//

#include "RecvNode.h"

RecvNode::RecvNode(const MSG_LEN_TYPE len, const MSG_ID_TYPE id)
    :MsgNode(len),
    m_msg_id(id)
{}

RecvNode::~RecvNode()
{
    delete [] m_data;
}

MSG_ID_TYPE RecvNode::Get_Msg_Id() const
{
    return m_msg_id;
}
