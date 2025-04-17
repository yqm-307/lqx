#pragma once
#include <bbt/pollevent/EvThread.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>

namespace service::gateway
{

class GatewayService
{
public:
    static GatewayService& GetInstance();
    ~GatewayService() = default;

    void Start();

    void Stop();
private:
    GatewayService() = default;

    void WaitToExit();
    void OnUpdateCo();
    void CoUpdateDatabaseInfo();

private:
    volatile bool m_is_running{true};

    std::shared_ptr<bbt::pollevent::EvThread>   m_client_thread{nullptr};    
    std::shared_ptr<monitor::MonitorClient>     m_monitor_client{nullptr};

    std::shared_ptr<bbt::pollevent::Event>      m_signal_event{nullptr};
    std::shared_ptr<bbt::pollevent::Event>      m_sigquit_event{nullptr};
};

} // namespace service::database