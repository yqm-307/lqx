#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace service::scene
{

class SceneConfig
{
public:
    static std::unique_ptr<SceneConfig>& GetInstance();
    ~SceneConfig() = default;
    void LoadConfig(const std::string& config_file);
private:
    SceneConfig() = default;

public:
    std::string m_service_name;
    int m_scene_id;
    std::string m_ip;
    short m_port{0};
    int m_connection_timeout{0};
};

}