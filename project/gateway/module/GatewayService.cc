#include <signal.h>
#include <gateway/module/GatewayService.hpp>
#include <monitor/monitorclient/MonitorClientConfig.hpp>

namespace service::gateway
{

using namespace bbt::pollevent;

GatewayService& GatewayService::GetInstance()
{
    static GatewayService instance;
    return instance;
}

void GatewayService::Start()
{
    auto& monitor_client_config = monitor::MonitorClientConfig::GetInstance();

    m_client_thread = std::make_shared<bbt::pollevent::EvThread>();
    m_monitor_client = std::make_shared<monitor::MonitorClient>(m_client_thread, "database", monitor_client_config->m_ip, monitor_client_config->m_port);

    // 初始化monitor client
    if (auto err = m_monitor_client->RunInEvThread(); err.has_value())
    {
        BBT_FULL_LOG_ERROR("monitor client run in ev thread failed! %s", err->What().c_str());
        return;
    }
    BBT_BASE_LOG_INFO("monitor client run in ev thread success! ip=%s port=%d", monitor_client_config->m_ip.c_str(), monitor_client_config->m_port);

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
    m_client_thread->Start();

    bbtco [&](){ OnUpdateCo(); };

    WaitToExit();
}

void GatewayService::Stop()
{
    m_is_running = false;
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