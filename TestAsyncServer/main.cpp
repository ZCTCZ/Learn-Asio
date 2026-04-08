#include <iostream>
#include  <boost/asio.hpp>

int main()
{
    try
    {
        constexpr int MAX_LEN = 1024 * 10; // 发送的数据包的最大长度
        constexpr int HEAD_LEN = 4; // 数据包首部长度
        boost::asio::io_context ioc;
        boost::system::error_code ec;
        boost::asio::ip::tcp::socket socket(ioc);
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address("127.0.0.1"), 60001);
        socket.connect(ep, ec);
        if (ec)
        {
            std::cerr << "connect error!"
                << " error_code = " << ec.value()
                << " error_message = " << ec.message() << std::endl;
        }

        std::thread recv_thread([&socket]()
        {
            try
            {
                char data_len[HEAD_LEN] = {'\0'};
                char buf[MAX_LEN] = {'\0'};
                for (;;)
                {
                    /// 先读取 HEAD_LEN 字节的报文首部
                    boost::asio::read(socket, boost::asio::buffer(data_len, HEAD_LEN));
                    uint32_t len = 0;
                    memcpy(&len, data_len, HEAD_LEN);
                    len = boost::asio::detail::socket_ops::network_to_host_long(len);
                    /// 再读取 data_len 长度的报文体
                    boost::asio::read(socket, boost::asio::buffer(buf, len));

                    std::cout << buf << std::endl;

                    memset(buf, '\0', len);
                    memset(data_len, '\0', HEAD_LEN);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            catch (boost::system::system_error &error)
            {
                std::cerr << "Occurred Error"
                << " error_code = " << error.code()
                << " error_message = " << error.what() << std::endl;
            }
        });

        std::thread write_thread([&socket]()
        {
            try
            {
                long long cnt {0};
                char send_data[MAX_LEN] = {'\0'};
                for (;;)
                {
                    std::string data = "Hello World!" + std::to_string(cnt++);
                    uint32_t data_len = data.size();
                    uint32_t tmp = boost::asio::detail::socket_ops::host_to_network_long(data_len);
                    memcpy(send_data, &tmp, HEAD_LEN);
                    memcpy(send_data + HEAD_LEN, data.c_str(), data_len);
                    boost::asio::write(socket, boost::asio::buffer(send_data, data_len + HEAD_LEN));

                    memset(send_data, '\0', data_len + HEAD_LEN);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));// 休眠10毫秒
                }
            }
            catch (boost::system::system_error &error)
            {
                std::cerr << "Occurred Error"
                << " error_code = " << error.code()
                << " error_message = " << error.what() << std::endl;
            }
        });

        recv_thread.join();
        write_thread.join();
    }
    catch (boost::system::system_error &error)
    {
        std::cerr << "Occurred Error"
        << " error_code = " << error.code()
        << " error_message = " << error.what() << std::endl;
    }

    return 0;
}
