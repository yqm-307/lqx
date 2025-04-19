#pragma once
#include <bbt/rpc/RpcServer.hpp>
#include <bbt/pollevent/EvThread.hpp>
#include <monitor/Define.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <monitor/module/MonitorServer.hpp>

namespace service::monitor
{

class MonitorService
{
public:
    static std::unique_ptr<MonitorService>& GetInstance();
    ~MonitorService() = default;

    void Start();
    void Stop();


private:
    MonitorService() = default;
    void InitMonitorServer();
    void OnUpdateCo();
    void WaitToExit();

private:
    volatile bool m_is_running{true};
    std::shared_ptr<bbt::pollevent::EvThread> m_client_thread{nullptr};

    std::shared_ptr<bbt::pollevent::Event> m_signal_event{nullptr};
    std::shared_ptr<bbt::pollevent::Event> m_sigquit_event{nullptr};

    std::shared_ptr<MonitorServer> m_monitor_server{nullptr};
};

} // namespace service::monitor