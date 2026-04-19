//
// Created by AdminCZ on 2026/4/18.
//

#ifndef THREADPOOL_ASYNCSERVER_IOTHREADPOOL_H
#define THREADPOOL_ASYNCSERVER_IOTHREADPOOL_H
#include <boost/asio.hpp>
#include  "Singleton.h"

class IOThreadPool : public Singleton<IOThreadPool>{
    friend class Singleton;
public:
    ~IOThreadPool() override;
    boost::asio::io_context& GetContext();
    void stop();
private:
    explicit IOThreadPool(size_t nums = std::thread::hardware_concurrency());

    boost::asio::io_context m_ioc;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work_guard;
    std::vector<std::thread> m_vec_threads;
};



#endif //THREADPOOL_ASYNCSERVER_IOTHREADPOOL_H
