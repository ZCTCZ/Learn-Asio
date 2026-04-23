#include <boost/asio.hpp>
#include <iostream>


boost::asio::awaitable<void> echo(boost::asio::ip::tcp::socket socket)
{
    char data[1024]{'\0'};
    try {
        for (;;) {
            const auto transfer_len =
                co_await socket.async_read_some(boost::asio::buffer(data, sizeof data - 1), boost::asio::use_awaitable);
            std::cout << "receive from [" << socket.remote_endpoint().address().to_string() << "] " << data << std::endl;

            co_await socket.async_write_some(boost::asio::buffer(data, transfer_len), boost::asio::use_awaitable);
            memset(data, '\0', sizeof data);
        }
    }
    catch (const boost::system::system_error& error) {
        if (error.code() == boost::asio::error::eof)
            std::cout << "client [" << socket.remote_endpoint().address().to_string() << "] disconnected" << std::endl;
        else
            std::cout << "error_code = " << error.code() << " error_message = " << error.what() << std::endl;
    }
}

boost::asio::awaitable<void> listener()
{
    try {
        /// 获取当前协程的执行器
        /// co_await 关键字是 C++ 协程中从 awaitable 对象提取值的唯一语法手段
        /// #boost::asio::this_coro::executor 是一个 Awaitable object
        auto executor = co_await boost::asio::this_coro::executor;

        auto acceptor = boost::asio::ip::tcp::acceptor(
            executor, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), 60001));

        for (;;) {
            /**
             * boost::asio::use_awaitable：告诉 Asio 以协程方式完成异步操作
             * co_await：暂停当前协程，直到有客户端连接进来；连接成功后恢复执行
             */
            auto socket = co_await acceptor.async_accept(boost::asio::use_awaitable);


            /// 传入的 #executor 本质上就是 #main 里面 #ioc 的执行器
            boost::asio::co_spawn(executor, echo(std::move(socket)), boost::asio::detached);
        }
    }
    catch (const boost::system::system_error& error) {
        puts("Occurred Error");
        std::cout << "error_code = " << error.code() << " error_message = " << error.what() << std::endl;
    }
}


int main()
{
    boost::asio::io_context ioc(1); // 传入 1 表示只会在单线程里运行 run()
    /**
     * SIGINT — 中断信号（Ctrl+C）
     * SIGTERM — 终止信号（正常 kill）
     * SIGABRT — 异常终止信号（abort() 调用触发）
     */
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM, SIGABRT);
    signals.async_wait([&ioc](const boost::system::error_code& error, int signal_num) {
        if (error) {
            puts("Occurred Error When Async_wait Signal");
            std::cout << "error_code = " << error.value() << " error_message = " << error.message() << std::endl;
        }
        else {
            std::cout << "capture signal:" << signal_num << std::endl;
        }

        puts("server quit!");
        ioc.stop();
    });

    /// 将 #listener() 作为协程挂载到 io_context 上
    /// 这个时候并不会调用 #listener() 函数。只有当 启动 run() 函数之后，底层的事件循环才会取出这个协程开始执行
    boost::asio::co_spawn(ioc, listener(), boost::asio::detached);

    ioc.run();
    return 0;
}
