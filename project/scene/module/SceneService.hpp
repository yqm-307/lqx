#pragma once
#include <bbt/pollevent/EventLoop.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <scene/module/world/WorldScene.hpp>
#include <scene/module/SceneServer.hpp>

namespace service::scene
{

class SceneService
{
public:
    static std::unique_ptr<SceneService>& GetInstance();
    ~SceneService() = default;

    ErrOpt Start();
    void Stop();
private:
    SceneService() = default;
    ErrOpt InitSceneServer();
    ErrOpt InitSignalEvent();
    ErrOpt InitMonitorClient();
    ErrOpt InitWorldScene();
    void WorldSceneRun();

private:
    bbt::pollevent::EventLoop::SPtr             m_event_loop{nullptr};

    std::shared_ptr<bbt::pollevent::EvThread>   m_thread{nullptr};
    std::shared_ptr<monitor::MonitorClient>     m_monitor_client{nullptr};
    std::shared_ptr<SceneServer>                m_scene_server{nullptr};

    std::shared_ptr<bbt::pollevent::Event>      m_signal_event{nullptr};
    std::shared_ptr<bbt::pollevent::Event>      m_sigquit_event{nullptr};

    std::shared_ptr<WorldScene>                 m_world_scene{nullptr};
};

} // namespace service::scene