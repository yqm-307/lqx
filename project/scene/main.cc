#include <monitor/monitorclient/MonitorClient.hpp>
#include <scene/module/SceneConfig.hpp>
#include <scene/module/SceneService.hpp>

using namespace service;

void LogInit()
{
    auto& scene_config = scene::SceneConfig::GetInstance();
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix(scene_config->m_service_name);
}

void ConfigInit(const std::string& config_file)
{
    auto& config = monitor::MonitorClientConfig::GetInstance();
    config->LoadConfig(config_file);
    
    auto& scene_config = scene::SceneConfig::GetInstance();
    scene_config->LoadConfig(config_file);
}

int main(int argc, char** argv)
{
    AssertWithInfo(argc == 2, "please input config file!");
    ConfigInit(argv[1]);
    LogInit();
    
    if (auto err = scene::SceneService::GetInstance()->Start(); err.has_value())
    {
        BBT_BASE_LOG_ERROR("SceneService start failed: %s", err->What().c_str());
        return -1;
    }

    return 0;
}