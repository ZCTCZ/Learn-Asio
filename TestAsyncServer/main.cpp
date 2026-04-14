#include <iostream>
#include  <boost/asio.hpp>

int main()
{
    try
    {
        constexpr int MAX_LEN = 1024 * 10; // 发送的数据包的最大长度
        constexpr int HEAD_DATA_LEN = 2; // 数据包首部里表示发送的数据的长度的字段所占字节数
        constexpr int HEAD_ID_LEN = 2; // 数据包首部里表示发送的数据的id的字段所占字节数
        constexpr int HEAD_TOTAL_LEN = 4;// 数据包首部长度
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
                char head[HEAD_TOTAL_LEN + 1] = {'\0'};
                char buf[MAX_LEN + 1] = {'\0'};
                for (;;)
                {
                    boost::asio::read(socket, boost::asio::buffer(head, HEAD_TOTAL_LEN));
                    uint16_t msg_id = 0;
                    memcpy(&msg_id, head, HEAD_ID_LEN);
                    msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);

                    uint16_t data_len = 0;
                    memcpy(&data_len, head + HEAD_ID_LEN, HEAD_DATA_LEN);
                    data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);

                    boost::asio::read(socket, boost::asio::buffer(buf, data_len));

                    std::cout << '[' << msg_id << ']' << buf << std::endl;

                    memset(buf, '\0', data_len);
                    memset(head, '\0', HEAD_TOTAL_LEN);
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
                long long id = 1001;
                char send_data[MAX_LEN] = {};
                for (;;)
                {
                    /// 构造首部里的id字段
                    uint16_t tmp = boost::asio::detail::socket_ops::host_to_network_short(id++);
                    memcpy(send_data, &tmp, HEAD_ID_LEN);

                    /// 构造待发送的数据
                    std::string data = "你好 World!" + std::to_string(cnt++);

                    /// 构造首部里表示数据长度的字段
                    uint16_t data_len = data.size();
                    tmp = boost::asio::detail::socket_ops::host_to_network_short(data_len);
                    memcpy(send_data + HEAD_ID_LEN, &tmp, HEAD_DATA_LEN);

                    /// 构造数据报的报体
                    memcpy(send_data + HEAD_TOTAL_LEN, data.c_str(), data_len);
                    boost::asio::write(socket, boost::asio::buffer(send_data, data_len + HEAD_TOTAL_LEN));

                    memset(send_data, '\0', data_len + HEAD_TOTAL_LEN);
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
