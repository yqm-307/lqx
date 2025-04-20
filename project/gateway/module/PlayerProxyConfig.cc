#include <gateway/module/PlayerProxyConfig.hpp>

namespace service::gateway
{

std::unique_ptr<PlayerProxyConfig>& PlayerProxyConfig::GetInstance()
{
    static std::unique_ptr<PlayerProxyConfig> instance(new PlayerProxyConfig());
    return instance;
}

void PlayerProxyConfig::LoadConfig(const std::string& config_file)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(config_file, pt);

    m_port = pt.get<short>("playerproxy.port");
}



}