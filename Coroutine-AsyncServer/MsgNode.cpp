//
// Created by AdminCZ on 2026/4/23.
//
#include "MsgNode.h"

MsgNode::MsgNode(unsigned int len) :
    m_total_len(len)
{
    if (m_total_len > 0) {
        m_data.resize(len + 1, '\0');
    }
}

void MsgNode::Clear() noexcept
{
    std::fill_n(m_data.begin(), m_cur_len, '\0');
    m_cur_len = 0;
}

char* MsgNode::GetData() noexcept
{
    return m_data.data();
}

unsigned int MsgNode::GetTotalLen() const noexcept
{
    return m_total_len;
}

