#pragma once
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace service::gateway
{

class PlayerProxyConfig
{
public:
    static std::unique_ptr<PlayerProxyConfig>& GetInstance();
    ~PlayerProxyConfig() = default;
    void LoadConfig(const std::string& config_file);
private:
    PlayerProxyConfig() = default;

public:
    short m_port{0};
};

}