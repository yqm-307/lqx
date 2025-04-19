#pragma once
#include <scene/module/Define.hpp>
#include <bbt/ecs/scene/Scene.hpp>

namespace service::scene
{

class WorldScene
{
public:
    WorldScene() = default;
    ~WorldScene() = default;

    ErrOpt  Init();
    ErrOpt  WorldRun();
    void    WorldStop();
private:
    ErrOpt  OnWorldStop();
private:
    volatile bool                       m_is_running{true};

    std::shared_ptr<bbt::ecs::Scene>    m_scene{nullptr};
};

} // namespace service::scene