//
// Created by AdminCZ on 2026/4/5.
//

#ifndef ASYNCSERVER_MSGNODE_H
#define ASYNCSERVER_MSGNODE_H

#include <cstddef>
#include <cstring>
#include "const.h"

class MsgNode {
public:
    explicit MsgNode(MSG_LEN_TYPE len);
    const char *GetData() const;
    char *GetData();
    size_t Get_Cur_Len() const;
    size_t Get_Total_Len() const;

    void Clear();
    virtual ~MsgNode();
protected:
    char *m_data;
    size_t m_cur_len; // 表示当前已处理的长度
    size_t m_total_len; // 字符串总长度，不包括末尾的'\0'
};



#endif //ASYNCSERVER_MSGNODE_H
