//
// Created by AdminCZ on 2026/4/23.
//

#ifndef COROUTINE_ASYNCSERVER_MSGNODE_H
#define COROUTINE_ASYNCSERVER_MSGNODE_H
#include "constant.h"
#include <vector>

class MsgNode {
public:
    explicit MsgNode(unsigned int len);
    virtual ~MsgNode() = default;
    void Clear() noexcept;
    char* GetData() noexcept;
    unsigned int GetTotalLen() const noexcept;

protected:
    std::vector<char> m_data;
    unsigned int m_cur_len{0};
    unsigned int m_total_len{0};
};

#endif // COROUTINE_ASYNCSERVER_MSGNODE_H
