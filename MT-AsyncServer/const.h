//
// Created by AdminCZ on 2026/4/10.
//

#ifndef ASYNCSERVER_CONST_H
#define ASYNCSERVER_CONST_H
#include <cstdint>
#include <chrono>

using MSG_LEN_TYPE = uint16_t;
using MSG_ID_TYPE = uint16_t;

/// 定义各种消息类型
enum class MSG_TYPE : MSG_ID_TYPE
{
    MAG_HELLO_WORLD = 1001, // Hello World 类型的消息
    INVALID_MSG_ID = 0xFFFF // #RecvNoded 的 #m_msg_id 字段的默认值，同时也是非法值
};

constexpr int HEAD_ID_LEN = sizeof(MSG_ID_TYPE);
constexpr int HEAD_DATA_LEN = sizeof(MSG_LEN_TYPE);
constexpr int HEAD_TOTAL_LEN = HEAD_ID_LEN + HEAD_DATA_LEN;

constexpr size_t BUF_SIZE = 1024; // 用户的读缓冲区最大大小
constexpr int MAX_LENGTH = 1024 * 8; // 单条数据大小不能超过 8k 字节
constexpr int MAX_QUE_LEN = 1000; // 发送队列最大长度
constexpr int MAX_MSG_QUE_CAPACITY = 200; //消息队列的最大容量，当超过这个值的时候，会触发和备用队列的交换操作

const std::chrono::milliseconds MAX_DELAY_MS{500}; // 最大延迟500ms


#endif //ASYNCSERVER_CONST_H
