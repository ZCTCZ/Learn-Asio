//
// Created by AdminCZ on 2026/4/4.
//

#ifndef ASYNCSERVER_SESSION_H
#define ASYNCSERVER_SESSION_H

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/// 前向声明，避免循环包含
/// 不需要 include <Server.h>
class Server;

/// Session 类继承自模板类 std::enable_shared_from_this<>
class Session : public std::enable_shared_from_this<Session>{
    friend class Server;
    static  constexpr size_t BUF_SIZE = 1024;
public:
    explicit Session(boost::asio::io_context& ioc, Server *server_ptr)
        : m_socket(ioc),// 根据传入的上下文构建 socket
        m_server_ptr(server_ptr)
    {
        const auto uuid = boost::uuids::random_generator()();
        m_uuid = boost::uuids::to_string(uuid);
    }

    void start();
    /**
     *
     * @return boost::asio::ip::tcp::socket 不支持拷贝，返回引用
     */
    boost::asio::ip::tcp::socket& Socket() {return m_socket;}
private:
    void handle_read(const boost::system::error_code &ec, size_t transfer_bytes);
    void handle_write(const boost::system::error_code &ec);

    boost::asio::ip::tcp::socket m_socket;
    char m_data[BUF_SIZE] = {'\0'};
    std::string m_uuid;

    /// 不可以使用 shared_ptr，会造成循环引用问题
    /// 在现代 C++ 里，裸指针表达的就是 “非拥有引用” 的关系
    Server* m_server_ptr;
};



#endif //ASYNCSERVER_SESSION_H
