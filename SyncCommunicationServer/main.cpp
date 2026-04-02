#include <iostream>
#include <unordered_set>
#include <boost/asio.hpp>

using SOCKET_PTR = std::shared_ptr<boost::asio::ip::tcp::socket>;
using THREAD_PTR = std::shared_ptr<std::thread>;
std::unordered_set<THREAD_PTR> thread_set;

/**
 * @brief 线程函数，在单独的子线程里进行
 * @param socket_ptr
 */
void session(SOCKET_PTR socket_ptr)
{
    try
    {
        constexpr ssize_t BUF_SIZE = 1024;
        char buf[BUF_SIZE] = {};
        boost::system::error_code ec;
        const std::string client_ip_str = socket_ptr->remote_endpoint().address().to_string();

        for (;;)
        {
            size_t read_bytes = socket_ptr->read_some(boost::asio::buffer(buf, BUF_SIZE), ec);
            if (ec)
            {
                if (ec == boost::asio::error::eof) // 对端关闭
                {
                    std::cout << "Connection close by peer : " << client_ip_str << std::endl;
                    return ;
                }

                std::cout << "Read from [" << client_ip_str << "] Occurred Error"
                    << " error_code = " << ec.value()
                    << " error_message = " << ec.message() << std::endl;

                return;
            }

            std::cout << "receive from [" << client_ip_str << "] : ";
            std::cout.write(buf, read_bytes);
            std::cout << std::endl;

            // 将读取到的数据原封不动地发送回去
            size_t write_bytes = boost::asio::write(*socket_ptr, boost::asio::buffer(buf, read_bytes), ec);
            if (ec)
            {
                std::cout << "Write to [" << client_ip_str << "] Occurred Error"
                    << " error_code = " << ec.value()
                    << " error_message = " << ec.message() << std::endl;

                return ;
            }

            memset(buf, '\0', read_bytes);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Error Occurred " << e.what() << std::endl;
    }
}


void server(boost::asio::io_context& ioc, const unsigned short server_port)
{
    boost::system::error_code ec;
    auto acceptor = boost::asio::ip::tcp::acceptor(ioc);

    // 在底层创建打开一个用于监听的 socket
    // socket(AF_INET6, SOCK_STREAM, 0);
    acceptor.open(boost::asio::ip::tcp::v4(), ec);
    if (ec)
    {
        std::cerr << "Open Socket Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        return ;
    }

    acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), server_port), ec);
    if (ec)
    {
        std::cerr << "Bind Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        return ;
    }

    acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        std::cerr << "Listen Error"
        << " error_code = " << ec.value()
        << " error_message = " << ec.message() << std::endl;
        return ;
    }

    for (;;)
    {
        auto socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>(ioc);
        // 阻塞等待请求
        acceptor.accept(*socket_ptr);

        // 创建子线程，处理 I/O 通信
        auto thread_ptr = std::make_shared<std::thread>(session, socket_ptr);
        thread_set.insert(thread_ptr); // 增加引用计数，防止子线程对象析构
    }
}

int main() {
    try
    {
        constexpr unsigned short local_port_num = 60001;
        boost::asio::io_context ioc;
        server(ioc, local_port_num);

        for (auto &e : thread_set)
        {
            e->join();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error Occurred: " << e.what() << std::endl;
    }

    return 0;
}
