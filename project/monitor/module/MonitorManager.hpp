#pragma once
#include <monitor/Define.hpp>
#include <bbt/core/clock/Clock.hpp>

namespace service::monitor
{

struct MonitorInfo
{
    std::string name;
    std::string ip;
    short port;
    bbt::core::clock::Timestamp<> last_active_time;
};

class MonitorManager
{
public:
    static std::unique_ptr<MonitorManager>& GetInstance();

    MonitorManager();
    ~MonitorManager();

    void Enliven(const std::string& uuid, const std::string &name);
    void DebugPrint();

private:
    std::mutex m_all_opt_mtx;
    std::unordered_map<std::string, MonitorInfo>
        m_enliven_map; // uuid -> name
};

} // namespace service::monitor