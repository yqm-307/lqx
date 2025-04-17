#pragma once
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace service::monitor
{

class MonitorClientConfig
{
public:
    static std::unique_ptr<MonitorClientConfig>& GetInstance();
    ~MonitorClientConfig() = default;
    void LoadConfig(const std::string& config_file);
private:
    MonitorClientConfig() = default;

public:
    std::string m_ip;
    short m_port{0};
    int m_connect_timeout{0};
    int m_client_timeout{0};
    int m_feed_dog_interval{0};
};

}