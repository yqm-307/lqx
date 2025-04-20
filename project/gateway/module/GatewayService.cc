#include <signal.h>
#include <gateway/module/GatewayService.hpp>
#include <gateway/module/PlayerProxyConfig.hpp>

namespace service::gateway
{

using namespace bbt::pollevent;

GatewayService& GatewayService::GetInstance()
{
    static GatewayService instance;
    return instance;
}

ErrOpt GatewayService::Start()
{
    g_scheduler->Start();
    m_client_thread = std::make_shared<bbt::pollevent::EvThread>();

    if (auto err = InitMonitorClient(); err.has_value())
        return err;
    if (auto err = InitSignalEvent(); err.has_value())
        return err;
    if (auto err = InitPlayerProxy(); err.has_value())
        return err;

    m_client_thread->Start();
    bbtco [&](){ OnUpdateCo(); };

    WaitToExit();
    g_scheduler->Stop();
    return std::nullopt;
}

void GatewayService::Stop()
{
    m_is_running = false;
}

ErrOpt GatewayService::InitMonitorClient()
{
    if (m_monitor_client)
        return std::nullopt;

    auto& config = monitor::MonitorClientConfig::GetInstance();
    m_monitor_client = std::make_shared<monitor::MonitorClient>(m_client_thread, "gateway", config->m_ip, config->m_port);
    if (auto err = m_monitor_client->RunInEvThread(); err.has_value())
    {
        BBT_FULL_LOG_ERROR("monitor client run in ev thread failed! %s", err->What().c_str());
        return err;
    }
    BBT_BASE_LOG_INFO("monitor client run in ev thread success! ip=%s port=%d", config->m_ip.c_str(), config->m_port);

    return std::nullopt;
}

ErrOpt GatewayService::InitPlayerProxy()
{
    if (m_player_proxy)
        return std::nullopt;

    auto& config = PlayerProxyConfig::GetInstance();
    m_player_proxy = std::make_shared<PlayerProxy>();
    if (auto err = m_player_proxy->Init(m_client_thread, "0.0.0.0", config->m_port); err.has_value())
        return err;
    m_player_proxy->Start();

    return std::nullopt;
}

ErrOpt GatewayService::InitSignalEvent()
{
    // 初始化信号监听
    m_signal_event = m_client_thread->RegisterEvent(SIGINT, EventOpt::SIGNAL | EventOpt::PERSIST, [this](auto, auto, auto){
        BBT_BASE_LOG_INFO("SIGINT received, exiting...");
        Stop();
    });

    m_signal_event->StartListen(0);

    m_sigquit_event = m_client_thread->RegisterEvent(SIGQUIT, EventOpt::SIGNAL | EventOpt::PERSIST, [this](auto, auto, auto){
        BBT_BASE_LOG_INFO("SIGQUIT received, exiting...");
        Stop();
    });

    m_sigquit_event->StartListen(0);

    return std::nullopt;
}

void GatewayService::WaitToExit()
{
    while (m_is_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_client_thread->Stop();
}

void GatewayService::OnUpdateCo()
{
    while (m_is_running)
    {
        bbtco_sleep(100);
        m_monitor_client->Update();
    }
}

void GatewayService::CoUpdateDatabaseInfo()
{
    auto [err, info] = m_monitor_client->GetServiceInfoCo("database");
    if (err.has_value())
    {
        BBT_FULL_LOG_ERROR("get database info failed! %s", err->What().c_str());
        return;
    }
    BBT_BASE_LOG_INFO("get database info success!");
}




} // namespace service::database