//
// Created by AdminCZ on 2026/4/11.
//

#ifndef ASYNCSERVER_SINGLETON_H
#define ASYNCSERVER_SINGLETON_H
#include <iostream>
#include <memory>
#include <mutex>

/// 单例模板类
template <typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(const Singleton&&) = delete;
    virtual ~Singleton() = 0;
    static std::shared_ptr<T> m_instance;

public:
    static std::shared_ptr<T>& GetInstance();
};

template <typename T>
Singleton<T>::~Singleton() = default;

template <typename T>
std::shared_ptr<T>& Singleton<T>::GetInstance()
{
    static std::once_flag once_flag;
    std::call_once(once_flag, []()
    {
        m_instance = std::shared_ptr<T>(new T());
    });
    return m_instance;
}

template <typename T>
std::shared_ptr<T> Singleton<T>::m_instance = nullptr;

#endif //ASYNCSERVER_SINGLETON_H
