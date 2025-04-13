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

void MonitorManager::Enliven(const std::string& uuid, const std::string &name)
{
    std::lock_guard<std::mutex> lock(m_all_opt_mtx);

    auto it = m_enliven_map.find(uuid);

    if (it != m_enliven_map.end())
    {
        it->second.last_active_time = bbt::core::clock::now();
    }
    else
    {
        MonitorInfo info;
        info.name = name;
        info.last_active_time = bbt::core::clock::now();
        m_enliven_map.insert(std::make_pair(uuid, info));
    }
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