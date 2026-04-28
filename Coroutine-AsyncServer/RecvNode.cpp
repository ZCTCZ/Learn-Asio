//
// Created by AdminCZ on 2026/4/23.
//

#include "RecvNode.h"

RecvNode::RecvNode(unsigned int len, MSG_TYPE id) :
    MsgNode(len), m_id(id)
{

}

MSG_TYPE RecvNode::GetMsgId() const noexcept
{
    return m_id;
}
