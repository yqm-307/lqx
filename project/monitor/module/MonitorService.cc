#include <signal.h>
#include <bbt/pollevent/EvThread.hpp>
#include <bbt/pollevent/Event.hpp>
#include <monitor/module/MonitorManager.hpp>
#include <monitor/module/MonitorService.hpp>
#include <monitor/module/MonitorConfig.hpp>
#include <protocol/Protocol.hpp>

namespace service::monitor
{

using namespace bbt::pollevent;

std::unique_ptr<MonitorService>& MonitorService::GetInstance()
{
    static std::unique_ptr<MonitorService> instance(new MonitorService());
    return instance;
}

void MonitorService::InitMonitorServer()
{
    if (m_monitor_server)
    {
        BBT_BASE_LOG_WARN("[MonitorService] MonitorServer already initialized");
        return;
    }

    auto& monitor_server_config = MonitorConfig::GetInstance();
    m_monitor_server = std::make_shared<MonitorServer>(
        m_client_thread,
        monitor_server_config->m_ip.c_str(),
        monitor_server_config->m_port,
        monitor_server_config->m_connection_timeout);
    
    if (auto err = m_monitor_server->Init(); err.has_value())
    {
        BBT_BASE_LOG_ERROR("[MonitorService] InitMonitorServer failed: %s", err->CWhat());
        return;
    }
}

void MonitorService::Start()
{
    g_scheduler->Start();

    m_client_thread = std::make_shared<bbt::pollevent::EvThread>();

    // 初始化 monitor server
    InitMonitorServer();

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

void MonitorService::Stop()
{
    m_is_running = false;
}


void MonitorService::OnUpdateCo()
{
    while (m_is_running)
    {
        bbtco_sleep(5000);
        MonitorManager::GetInstance()->DebugPrint();
    }
}

void MonitorService::WaitToExit()
{
    while (m_is_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_client_thread->Stop();
    m_monitor_server = nullptr;
    g_scheduler->Stop();
}

} // namespace service::monitor