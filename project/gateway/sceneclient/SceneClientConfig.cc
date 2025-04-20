#include <gateway/sceneclient/SceneClientConfig.hpp>

namespace service::gateway
{

std::unique_ptr<SceneClientConfig>& SceneClientConfig::GetInstance()
{
    static std::unique_ptr<SceneClientConfig> instance(new SceneClientConfig());
    return instance;
}

void SceneClientConfig::LoadConfig(const std::string& config_file)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(config_file, pt);

    m_ip = pt.get<std::string>("sceneclient.ip");
    m_port = pt.get<short>("sceneclient.port");
    m_connect_timeout = pt.get<int>("sceneclient.connect_timeout");
    m_client_timeout = pt.get<int>("sceneclient.connection_timeout");
}



}