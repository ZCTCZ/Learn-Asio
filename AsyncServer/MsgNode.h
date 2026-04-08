//
// Created by AdminCZ on 2026/4/5.
//

#ifndef ASYNCSERVER_MSGNODE_H
#define ASYNCSERVER_MSGNODE_H

#include <cstddef>
#include <cstdint>
#include <cstring>

class MsgNode {
    friend class Session;
public:
    /// 数据包的头部，用 HEAD_LENGTH 字节的空间，存放数据所占字节数
    static constexpr int HEAD_LENGTH = 4;

    explicit MsgNode(const char *msg, uint32_t data_len);
    explicit MsgNode(uint32_t max_len);
    void Clear();
    ~MsgNode();
private:
    char *m_data;
    size_t m_cur_len; // 表示当前已处理的长度
    size_t m_total_len; // 字符串总长度，不包括末尾的'\0'
};



#endif //ASYNCSERVER_MSGNODE_H
