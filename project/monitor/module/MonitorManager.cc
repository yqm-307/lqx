#include <monitor/module/MonitorManager.hpp>

namespace service::monitor
{

std::unique_ptr<MonitorManager>& MonitorManager::GetInstance()
{
    static std::unique_ptr<MonitorManager> instance(new MonitorManager());
    return instance;
}

MonitorManager::MonitorManager()
{
}

MonitorManager::~MonitorManager()
{
}

void MonitorManager::Enliven(const protocol::FeedDogReq& req)
{
    std::lock_guard<std::mutex> lock(m_all_opt_mtx);

    auto& name = std::get<1>(req);
    auto it = m_enliven_map.find(name);

    MonitorInfo* info = nullptr;

    if (it == m_enliven_map.end())
    {
        m_enliven_map.insert(std::make_pair(name, MonitorInfo()));
        info = &m_enliven_map[name];
    }
    else
        info = &it->second;

    info->uuid = std::get<0>(req);
    info->name = std::get<1>(req);
    info->ip = std::get<2>(req);
    info->port = std::get<3>(req);
    info->last_active_time = bbt::core::clock::now();

}

std::optional<MonitorInfo> MonitorManager::GetServiceInfo(const std::string& service_name)
{
    std::lock_guard<std::mutex> lock(m_all_opt_mtx);

    auto it = m_enliven_map.find(service_name);
    if (it == m_enliven_map.end())
    {
        return std::nullopt;
    }

    return it->second;
}


void MonitorManager::DebugPrint()
{
    std::lock_guard<std::mutex> lock(m_all_opt_mtx);

    for (const auto& [uuid, info] : m_enliven_map)
    {
        BBT_FULL_LOG_DEBUG("[MonitorManager] uuid: %s, name: %s, last_active_time: %s",
                           uuid.c_str(),
                           info.name.c_str(),
                           bbt::core::clock::tostr(info.last_active_time).c_str());
    }
}


} // namespace service::monitor