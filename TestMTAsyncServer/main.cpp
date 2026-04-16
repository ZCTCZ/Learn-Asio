#include <iostream>
#include <boost/asio.hpp>

constexpr int THREADNUMS = 100;
constexpr int TIMES = 500;
constexpr int MAX_LEN = 1024 * 10; // 发送的数据包的最大长度
constexpr int HEAD_DATA_LEN = 2; // 数据包首部里表示发送的数据的长度的字段所占字节数
constexpr int HEAD_ID_LEN = 2; // 数据包首部里表示发送的数据的id的字段所占字节数
constexpr int HEAD_TOTAL_LEN = 4; // 数据包首部长度

struct process
{
    void operator()() const
    {
        try
        {
            boost::asio::io_context ioc;
            boost::asio::ip::tcp::socket socket(ioc);
            boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address("127.0.0.1"), 60001);
            boost::system::error_code error;
            socket.connect(ep, error);
            if (error)
            {
                std::cerr << "Occurred Error When Connect"
                    << " error_code = " << error.value()
                    << " error_message = " << error.message() << std::endl;
                return;
            }

            uint64_t cnt = 0;
            uint16_t id = 1001;
            char sendData[MAX_LEN + 1] = {'\0'};
            char readBuffer[MAX_LEN + 1] = {'\0'};
            char headBuffer[HEAD_TOTAL_LEN + 1] = {'\0'};
            const std::string str("Hello World ");

            for (int i = 0; i < TIMES; ++i)
            {
                /// 发送数据
                uint16_t net_id = boost::asio::detail::socket_ops::host_to_network_short(id);
                memcpy(sendData, &net_id, HEAD_ID_LEN);

                auto msg = str + std::to_string(cnt++);
                uint16_t net_len = boost::asio::detail::socket_ops::host_to_network_short(msg.size());
                memcpy(sendData + HEAD_ID_LEN, &net_len, HEAD_DATA_LEN);

                memcpy(sendData + HEAD_TOTAL_LEN, msg.c_str(), msg.size());
                boost::asio::write(socket, boost::asio::buffer(sendData, HEAD_TOTAL_LEN + msg.size()));
                memset(sendData, '\0', HEAD_TOTAL_LEN + msg.size());

                /// 接收数据
                boost::asio::read(socket, boost::asio::buffer(headBuffer, HEAD_TOTAL_LEN));
                uint16_t host_id = 0;
                memcpy(&host_id, headBuffer, HEAD_ID_LEN);
                host_id = boost::asio::detail::socket_ops::network_to_host_short(host_id);

                uint16_t host_len = 0;
                memcpy(&host_len, headBuffer + HEAD_ID_LEN, HEAD_DATA_LEN);
                host_len = boost::asio::detail::socket_ops::network_to_host_short(host_len);

                boost::asio::read(socket, boost::asio::buffer(readBuffer, host_len));
                std::cout << '[' << host_id << ']' << readBuffer << std::endl;
                memset(headBuffer, '\0', HEAD_TOTAL_LEN);
                memset(readBuffer, '\0', host_len);

                /// 休息10ms，给其他线程争夺CPU的机会
                // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        catch (boost::system::system_error& error)
        {
            std::cerr << "Occurred Error"
                << " error_code = " << error.code().value()
                << " error_message = " << error.what() << std::endl;
        }
    }
};

int main()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < THREADNUMS; ++i)
    {
        threads.emplace_back(process());
        // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }


    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << duration << std::endl;
    return 0;
}
