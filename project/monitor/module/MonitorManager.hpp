#pragma once
#include <monitor/Define.hpp>
#include <bbt/core/clock/Clock.hpp>
#include <protocol/MonitorProtocols.hpp>

namespace service::monitor
{

struct MonitorInfo
{
    std::string uuid;
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

    void Enliven(const protocol::FeedDogReq& req);
    std::optional<MonitorInfo> GetServiceInfo(const std::string& service_name);
    void DebugPrint();

private:
    std::mutex m_all_opt_mtx;
    std::unordered_map<std::string, MonitorInfo>
        m_enliven_map;  // service_name -> MonitorInfo
};

} // namespace service::monitor