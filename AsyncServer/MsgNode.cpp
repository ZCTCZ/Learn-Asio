//
// Created by AdminCZ on 2026/4/5.
//

#include "MsgNode.h"

#include <boost/asio/ip/host_name.hpp>

/// 构建存放待发送数据的 MsgNode
MsgNode::MsgNode(const char* msg, uint32_t data_len)
    :m_data(new char[MsgNode::HEAD_LENGTH + data_len + 1]), m_cur_len(0),
    m_total_len(MsgNode::HEAD_LENGTH + data_len)
{
    /// 将主机字节序转换成网络字节序
    uint32_t tmp = boost::asio::detail::socket_ops::host_to_network_long(data_len);
    memcpy(m_data, &tmp, MsgNode::HEAD_LENGTH);
    memcpy(m_data + MsgNode::HEAD_LENGTH, msg, data_len);
    m_data[m_total_len] = '\0';
}

/// 构建存放待接受数据的 MsgNode
MsgNode::MsgNode(uint32_t max_len)
    :m_data(new char[max_len + 1]), m_cur_len(0), m_total_len(max_len)
{
}

void MsgNode::Clear()
{
    memset(m_data, '\0', m_total_len);
    m_cur_len = 0;
}

MsgNode::~MsgNode()
{
    delete [] m_data; // m_data 是通过 new char[] 分配的
}
