#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace service::database
{

class Config
{
public:
    static std::unique_ptr<Config>& GetInstance();
    ~Config() = default;
    void LoadConfig(const std::string& config_file);
private:
    Config() = default;

public:
    std::string m_ip;
    short m_port{0};
    int m_connect_timeout{0};
    int m_client_timeout{0};
};

}