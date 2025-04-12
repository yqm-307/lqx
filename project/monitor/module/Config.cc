#include <monitor/module/Config.hpp>

namespace service::monitor
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

    m_ip = pt.get<std::string>("monitor.ip");
    m_port = pt.get<short>("monitor.port");
    m_connection_timeout = pt.get<int>("monitor.timeout");
}



}