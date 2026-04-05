//
// Created by AdminCZ on 2026/4/5.
//

#ifndef ASYNCSERVER_MSGNODE_H
#define ASYNCSERVER_MSGNODE_H

#include <cstddef>
#include <cstring>

class MsgNode {
    friend class Session;
public:
    explicit MsgNode(const char *msg, const size_t max_len)
    {
        m_data = new char[max_len];
        memcpy(m_data, msg, max_len);
        m_cur_len = 0;
        m_max_len = max_len;
    }

    ~MsgNode()
    {
        delete [] m_data; // m_data 是通过 new char[] 分配的
    }
private:
    MsgNode(const MsgNode&) = delete;
    MsgNode& operator=(const MsgNode&) = delete;

    char *m_data;
    size_t m_cur_len; // 表示当前已处理的长度
    size_t m_max_len;
};



#endif //ASYNCSERVER_MSGNODE_H
