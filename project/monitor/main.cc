#include <iostream>
#include <monitor/module/MonitorService.hpp>
#include <monitor/module/MonitorConfig.hpp>

using namespace service;

void LogInit()
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix("monitor");
}

void ConfigInit(const std::string& config_file)
{
    BBT_BASE_LOG_INFO("config file=%s", config_file.c_str());

    auto& config = monitor::MonitorConfig::GetInstance();
    config->LoadConfig(config_file);
    BBT_BASE_LOG_INFO("[monitor] ip=%s, port=%d, timeout=%d", config->m_ip.c_str(), config->m_port, config->m_connection_timeout);
}

int main(int argc, char* argv[])
{
    LogInit();

    AssertWithInfo(argc == 2, "please input config file!");
    ConfigInit(argv[1]);

    monitor::MonitorService::GetInstance()->Start();   
    return 0;
}