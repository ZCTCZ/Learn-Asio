//
// Created by AdminCZ on 2026/4/23.
//

#include "SendNode.h"
#include <boost/asio.hpp>

SendNode::SendNode(const char* msg, unsigned int msg_len, MSG_TYPE id) :
    MsgNode(msg_len + HEAD_TOTAL_LEN), m_id(id)
{
    ///1.构造消息报头
        /// 首先将 id 转成网络字节序
    MSG_ID_TYPE id_net_seq = boost::asio::detail::socket_ops::host_to_network_short(
        static_cast<MSG_ID_TYPE>(id));
    memcpy(m_data.data(), &id_net_seq, HEAD_ID_LEN);
    /// 再将 msg_len 转成网络字节序
    MSG_ID_TYPE msg_len_net_seq = boost::asio::detail::socket_ops::host_to_network_short(
        static_cast<MSG_ID_TYPE>(msg_len));
    memcpy(m_data.data() + HEAD_ID_LEN, &msg_len_net_seq, HEAD_DATA_LEN);

    ///2.构造消息体
    memcpy(m_data.data() + HEAD_TOTAL_LEN, msg, msg_len);
}


MSG_TYPE SendNode::GetMsgId() const noexcept
{
    return m_id;
}
