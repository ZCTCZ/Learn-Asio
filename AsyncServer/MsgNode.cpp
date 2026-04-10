//
// Created by AdminCZ on 2026/4/5.
//

#include "MsgNode.h"

#include <iostream>

MsgNode::MsgNode(MSG_LEN_TYPE len)
    :m_data(new char[len + 1]('\0')),
    m_cur_len(0),
    m_total_len(len)
{}

char* MsgNode::GetData()
{
    return m_data;
}

const char* MsgNode::GetData() const
{
    return m_data;
}

size_t MsgNode::Get_Cur_Len() const
{
    return m_cur_len;
}

size_t MsgNode::Get_Total_Len() const
{
    return m_total_len;
}

void MsgNode::Clear()
{
    memset(m_data, '\0', m_total_len);
    m_cur_len = 0;
}

MsgNode::~MsgNode()
{
}
