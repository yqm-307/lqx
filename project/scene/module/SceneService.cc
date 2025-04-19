#include <signal.h>
#include <scene/module/SceneService.hpp>
#include <scene/module/SceneConfig.hpp>

namespace service::scene
{

std::unique_ptr<SceneService>& SceneService::GetInstance()
{
    static std::unique_ptr<SceneService> instance(new SceneService());
    return instance;
}


ErrOpt SceneService::Start()
{
    g_scheduler->Start();  // 启动调度器
    m_thread = std::make_shared<bbt::pollevent::EvThread>();

    if (auto err = InitSceneServer(); err.has_value())  // 场景服务器初始化
        return err;
    if (auto err =InitSignalEvent(); err.has_value()) // 信号事件初始化
        return err;
    if (auto err = InitMonitorClient(); err.has_value())    // 监控客户端初始化
        return err;
    if (auto err = InitWorldScene(); err.has_value())  // 世界场景初始化
        return err;

    m_thread->Start();  // 开启后台线程

    bbtco [monitor_wkptr{m_monitor_client->weak_from_this()}](){
        while (monitor_wkptr.expired() == false)
        {
            bbtco_sleep(100);
            auto monitor_client = monitor_wkptr.lock();
            if (monitor_client)
                std::dynamic_pointer_cast<monitor::MonitorClient>(monitor_client)->Update();
        }
    };  // 启动协程

    WorldSceneRun();
    return std::nullopt;
}

void SceneService::Stop()
{
    m_world_scene->WorldStop();  // 停止世界场景
    if (m_thread)
    {
        m_thread->Stop();
        m_thread = nullptr;
    }
}

ErrOpt SceneService::InitSceneServer()
{
    if (m_scene_server)
        return std::nullopt;

    auto& config = SceneConfig::GetInstance();
    m_scene_server = std::make_shared<SceneServer>(m_thread, config->m_ip, config->m_port, config->m_connection_timeout);
    if (auto err = m_scene_server->Init(); err.has_value())
        return err;

    BBT_BASE_LOG_INFO("[SceneService] SceneServer started! %s:%d", config->m_ip.c_str(), config->m_port);
    return std::nullopt;
}

ErrOpt SceneService::InitSignalEvent()
{
    if (m_signal_event && m_sigquit_event)
        return std::nullopt;

    m_signal_event = m_thread->RegisterEvent(SIGINT, bbt::pollevent::EventOpt::SIGNAL | bbt::pollevent::EventOpt::PERSIST,
        [this](auto, auto, auto) {
            BBT_BASE_LOG_INFO("SIGINT received, exiting...");
            Stop();
        });
    if (m_signal_event->StartListen(0) != 0)
        return Errcode{"[SceneService] Failed to start signal event for SIGINT", ERR_UNKNOWN};

    m_sigquit_event = m_thread->RegisterEvent(SIGQUIT, bbt::pollevent::EventOpt::SIGNAL | bbt::pollevent::EventOpt::PERSIST,
        [this](auto, auto, auto) {
            BBT_BASE_LOG_INFO("SIGQUIT received, exiting...");
            Stop();
        });
    if (m_sigquit_event->StartListen(0) != 0)
        return Errcode{"[SceneService] Failed to start signal event for SIGQUIT", ERR_UNKNOWN};

    BBT_BASE_LOG_INFO("[SceneService] Signal events registered for SIGINT and SIGQUIT");
    return std::nullopt;
}

ErrOpt SceneService::InitMonitorClient()
{
    if (m_monitor_client)
        return std::nullopt;

    auto& config = monitor::MonitorClientConfig::GetInstance();
    m_monitor_client = std::make_shared<monitor::MonitorClient>(m_thread, config->m_service_name, config->m_ip, config->m_port);
    if (auto err = m_monitor_client->RunInEvThread(); err.has_value())
        return err;

    return std::nullopt;
}

ErrOpt SceneService::InitWorldScene()
{
    if (m_world_scene)
        return std::nullopt;

    m_world_scene = std::make_shared<WorldScene>();
    if (auto err = m_world_scene->Init(); err.has_value())
        return err;
    
    return std::nullopt;
}

void SceneService::WorldSceneRun()
{
    m_world_scene->WorldRun();
    m_thread->Stop();
    m_scene_server = nullptr;
    g_scheduler->Stop();  // 停止调度器

    BBT_BASE_LOG_INFO("[SceneService] SceneService stopped!");
}

} // namespace service::scene