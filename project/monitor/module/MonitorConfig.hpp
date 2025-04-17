#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace service::monitor
{

class MonitorConfig
{
public:
    static std::unique_ptr<MonitorConfig>& GetInstance();
    ~MonitorConfig() = default;
    void LoadConfig(const std::string& config_file);
private:
    MonitorConfig() = default;

public:
    std::string m_ip;
    short m_port{0};
    int m_connection_timeout{0};
};

}