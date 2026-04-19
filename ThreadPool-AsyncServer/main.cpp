#include <iostream>

#include "Server.h"
#include "IOThreadPool.h"

/**
 * 两种多线程模型的对比：
  负载均衡
  - 当前架构：io_context 自动将就绪事件分配给空闲线程，天然负载均衡
  - one loop per thread：连接创建时绑定到某个线程的 io_context，后续该连接的所有 I/O 都在该线程处理。如果分配不均匀，有的线程空闲有的繁忙，通常需要
  round-robin 或最少连接数策略来分配

  线程亲和性
  - 当前架构：同一个 session 的回调可能在不同 I/O 线程上执行，上下文切换较多，缓存亲和性差
  - one loop per thread：一个 session 的所有回调始终在同一线程执行，上下文切换少，缓存亲和性好

  Strand
  - 当前架构：每个 session 必须用 strand 保证回调串行
  - one loop per thread：单线程 io_context 天然串行，不需要 strand

  连接迁移
  - 当前架构：线程天然共享事件，没有迁移问题
  - one loop per thread：想把连接从一个线程迁移到另一个需要跨 io_context 移动 socket，很复杂

  总结：当前架构更简单、负载均衡更自然；one loop per thread 性能上限更高（减少锁和 strand
  开销、缓存友好），但需要更复杂的连接分配策略。连接数多且每个连接负载均匀时两者差距不大；连接数少但个别连接非常繁忙时，one loop per thread
  可能出现一个线程跑满而其他线程空闲的问题。
 *
 */
int main()
{
    try
    {
        /// 主线程里的 #ioc 专门用于监听信号
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM, SIGABRT);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_num)
        {
            if (error)
            {
                std::cerr << "Occurred Error When Capture Signal!"
                    << " error_code = " << error.value()
                    << " error_message = " << error.message() << std::endl;
            }
            else
            {
                std::cout << "Capture Signal:" << signal_num << '\n'
                    << "服务器完美退出！" << std::endl;
            }

            ioc.stop(); // 结束主线程的 run() 函数，最终导致整个main函数退出
        });

        const auto pool = IOThreadPool::GetInstance();
        Server s(pool->GetContext(), 60001);

        ioc.run(); // 启动底层的 I/O 多路复用
    }
    catch (boost::system::system_error& error)
    {
        std::cerr << "Occurred Error"
            << " error_code = " << error.code().value()
            << " error_message = " << error.what() << std::endl;
    }

    return 0;
}
