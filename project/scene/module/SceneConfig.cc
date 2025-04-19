#include <scene/module/SceneConfig.hpp>

namespace service::scene
{

std::unique_ptr<SceneConfig>& SceneConfig::GetInstance()
{
    static std::unique_ptr<SceneConfig> instance(new SceneConfig());
    return instance;
}

void SceneConfig::LoadConfig(const std::string& config_file)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(config_file, pt);

    m_service_name = pt.get<std::string>("scene.name");
    m_scene_id = pt.get<int>("scene.sceneid");
    m_ip = pt.get<std::string>("scene.ip");
    m_port = pt.get<short>("scene.port");
    m_connection_timeout = pt.get<int>("scene.connection_timeout");
}



}