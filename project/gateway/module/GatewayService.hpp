#pragma once
#include <bbt/pollevent/EvThread.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <gateway/module/PlayerProxy.hpp>

namespace service::gateway
{

class GatewayService
{
public:
    static GatewayService& GetInstance();
    ~GatewayService() = default;

    ErrOpt Start();

    void Stop();
private:
    GatewayService() = default;

    ErrOpt InitPlayerProxy();
    ErrOpt InitSignalEvent();
    ErrOpt InitMonitorClient();

    void WaitToExit();
    void OnUpdateCo();
    void CoUpdateDatabaseInfo();

private:
    volatile bool m_is_running{true};

    std::shared_ptr<bbt::pollevent::EvThread>   m_client_thread{nullptr};
    std::shared_ptr<monitor::MonitorClient>     m_monitor_client{nullptr};

    std::shared_ptr<bbt::pollevent::Event>      m_signal_event{nullptr};
    std::shared_ptr<bbt::pollevent::Event>      m_sigquit_event{nullptr};

    std::shared_ptr<PlayerProxy> m_player_proxy{nullptr}; // 玩家连接代理
};

} // namespace service::database