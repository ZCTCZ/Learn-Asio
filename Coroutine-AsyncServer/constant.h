//
// Created by AdminCZ on 2026/4/23.
//

#ifndef COROUTINE_ASYNCSERVER_CONST_H
#define COROUTINE_ASYNCSERVER_CONST_H
#include <cstdint>

using MSG_ID_TYPE = uint16_t; // 消息 ID 2 字节

/**
 * 定义消息类型
 */
enum class MSG_TYPE : MSG_ID_TYPE {
    MSG_HELLO_WORLD = 1001,
    MSG_INVALID_ID = 0XFFFF // 非法消息
};


constexpr uint16_t MSG_MAX_LEN = 1024 * 8; // 单条消息的最大长度 8KB
constexpr uint16_t HEAD_ID_LEN = 2; // 消息头中，消息 ID 这个字段的长度
constexpr uint16_t HEAD_DATA_LEN = 2; // 消息头中，表示 后续消息体长度 这个字段的长度
constexpr uint16_t HEAD_TOTAL_LEN = 4; // 消息头总长度

constexpr int MAX_SEND_QUE_LEN = 1000; // 待发送的消息队列里最多可以存放 1000 条数据
constexpr int LOGIC_MAX_MSG_QUE_LEN = 100; // 逻辑线程里消息队列的最大缓存， 超过这个值会唤醒逻辑线程，触发队列交换
constexpr int LOGIC_WAIT_TIME = 500; // 逻辑线程每隔 500 ms 醒来一次
#endif //COROUTINE_ASYNCSERVER_CONST_H
