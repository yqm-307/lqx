#include <database/module/Config.hpp>

namespace service::database
{

std::unique_ptr<Config>& Config::GetInstance()
{
    static std::unique_ptr<Config> instance(new Config());
    return instance;
}

void Config::LoadConfig(const std::string& config_file)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(config_file, pt);

    m_ip = pt.get<std::string>("monitorclient.ip");
    m_port = pt.get<short>("monitorclient.port");
    m_connect_timeout = pt.get<int>("monitorclient.connect_timeout");
    m_client_timeout = pt.get<int>("monitorclient.connection_timeout");
}



}