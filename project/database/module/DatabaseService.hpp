#pragma once
#include <bbt/pollevent/EvThread.hpp>
#include <bbt/coroutine/coroutine.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>

namespace service::database
{

class DatabaseService:
    public std::enable_shared_from_this<DatabaseService>
{
public:
    static std::unique_ptr<DatabaseService>& GetInstance();

    ~DatabaseService() = default;

    void Start();

    void Stop();
private:
    DatabaseService() = default;
    void WaitToExit();
    void OnUpdateCo();

private:
    volatile bool m_is_running{true};

    std::shared_ptr<bbt::pollevent::EvThread>   m_client_thread{nullptr};    
    std::shared_ptr<monitor::MonitorClient>     m_monitor_client{nullptr};

    std::shared_ptr<bbt::pollevent::Event>      m_signal_event{nullptr};
    std::shared_ptr<bbt::pollevent::Event>      m_sigquit_event{nullptr};
};

extern std::shared_ptr<DatabaseService> g_service;

} // namespace service::database