#pragma once
#include <scene/module/Define.hpp>

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
};

} // namespace service::scene