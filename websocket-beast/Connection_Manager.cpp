//
// Created by AdminCZ on 2026/5/2.
//

#include "Connection_Manager.h"

#include "Connection.h"

Connection_Manager& Connection_Manager::GetInstance()
{
    static Connection_Manager instance;
    return instance;
}

void Connection_Manager::AddConnection(std::shared_ptr<Connection> con_ptr)
{
    m_connection_map.insert({con_ptr->GetUUID(), std::move(con_ptr)});
}

std::shared_ptr<Connection> Connection_Manager::GetConnection(const std::string& uuid) const noexcept
{
    if (const auto it = m_connection_map.find(uuid); it != m_connection_map.end()) {
        return it->second;
    }

    return nullptr;
}

void Connection_Manager::RemoveConnection(const std::string& uuid) noexcept
{
    if (const auto it = m_connection_map.find(uuid); it != m_connection_map.end()) {
        m_connection_map.erase(it);
    }
}
