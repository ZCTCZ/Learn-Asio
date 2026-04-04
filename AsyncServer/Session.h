//
// Created by AdminCZ on 2026/4/4.
//

#ifndef ASYNCSERVER_SESSION_H
#define ASYNCSERVER_SESSION_H

#include <boost/asio.hpp>

class Session {
    static  constexpr size_t BUF_SIZE = 1024;
public:
    explicit Session(boost::asio::io_context& ioc)
        :m_socket(ioc) // 根据传入的上下文构建 socket
    {
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
};



#endif //ASYNCSERVER_SESSION_H
