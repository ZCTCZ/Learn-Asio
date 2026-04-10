//
// Created by AdminCZ on 2026/4/10.
//

#include "SendNode.h"
#include <boost/asio.hpp>


SendNode::SendNode(const char *msg, MSG_LEN_TYPE len, MSG_ID_TYPE id)
    :MsgNode(HEAD_TOTAL_LEN + len),
    m_msg_id(id)
{
    /// 将 id 转成 网路字节序，并将消息 id 放入报文首部
    const MSG_ID_TYPE id_net = boost::asio::detail::socket_ops::host_to_network_short(id);
    memcpy(m_data, &id_net, HEAD_ID_LEN);

    /// 将 len 转成网络字节序,并将消息长度放入报文首部
    const MSG_LEN_TYPE len_net = boost::asio::detail::socket_ops::host_to_network_short(len);
    memcpy(m_data + HEAD_ID_LEN, &len_net, HEAD_DATA_LEN);

    /// 将消息内容放入报文体
    memcpy(m_data + HEAD_TOTAL_LEN, msg, len);
}

SendNode::~SendNode()
{
    delete [] m_data;
}

MSG_ID_TYPE SendNode::Get_Msg_Id() const
{
    return m_msg_id;
}
