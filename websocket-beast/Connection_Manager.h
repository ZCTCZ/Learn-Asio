//
// Created by AdminCZ on 2026/5/2.
//

#ifndef WEBSOCKET_BEAST_CONNECTION_MANAGER_H
#define WEBSOCKET_BEAST_CONNECTION_MANAGER_H
#include <string>
#include <unordered_map>
#include <memory>
class Connection;

class Connection_Manager {
public:
    ~Connection_Manager() = default;
    static Connection_Manager& GetInstance();
    void AddConnection(std::shared_ptr<Connection> con_ptr);
    std::shared_ptr<Connection> GetConnection(const std::string& uuid) const noexcept;
    void RemoveConnection(const std::string& uuid) noexcept;

private:
    Connection_Manager() = default;

    std::unordered_map<std::string, std::shared_ptr<Connection>> m_connection_map;
};


#endif //WEBSOCKET_BEAST_CONNECTION_MANAGER_H
