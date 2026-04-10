//
// Created by AdminCZ on 2026/4/10.
//

#ifndef ASYNCSERVER_CONST_H
#define ASYNCSERVER_CONST_H
#include <cstdint>

using MSG_ID_TYPE = uint16_t;
using MSG_LEN_TYPE = uint16_t;

constexpr int HEAD_ID_LEN = sizeof(MSG_ID_TYPE);
constexpr int HEAD_DATA_LEN = sizeof(MSG_LEN_TYPE);
constexpr int HEAD_TOTAL_LEN = HEAD_ID_LEN + HEAD_DATA_LEN;

constexpr size_t BUF_SIZE = 1024; // 用户的读缓冲区最大大小
constexpr int MAX_LENGTH = 1024 * 8; // 单条数据大小不能超过 8k 字节
constexpr int MAX_QUE_LEN = 1000; // 消息队列最大长度

constexpr MSG_ID_TYPE INVALID_MSG_ID = 0xFFFF; // #RecvNoded 的 #m_msg_id 字段的默认值

#endif //ASYNCSERVER_CONST_H
