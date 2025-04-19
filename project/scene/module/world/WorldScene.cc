#include <scene/module/world/WorldScene.hpp>

namespace service::scene
{

ErrOpt WorldScene::Init()
{
    m_scene = std::make_shared<bbt::ecs::Scene>();
    m_scene->Init();
    return std::nullopt;
}

ErrOpt WorldScene::WorldRun()
{
    BBT_BASE_LOG_DEBUG("[WorldScene] WorldScene Running...");
    while (m_is_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        m_scene->Update();
    }

    OnWorldStop();
    return std::nullopt;
}

void WorldScene::WorldStop()
{
    m_is_running = false;
    BBT_BASE_LOG_INFO("[WorldScene] WorldScene stopped!");
}

ErrOpt WorldScene::OnWorldStop()
{
    return std::nullopt;
}

} // namespace service::scene