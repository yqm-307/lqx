#include <bbt/pollevent/EvThread.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <monitor/monitorclient/MonitorClientConfig.hpp>
#include <boost/process/environment.hpp>
#include <gateway/module/GatewayService.hpp>

using namespace service;

void LogInit()
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix("gateway");
}

void ConfigInit(const std::string& config_file)
{
    BBT_BASE_LOG_INFO("config file=%s", config_file.c_str());

    auto& config = monitor::MonitorClientConfig::GetInstance();
    config->LoadConfig(config_file);
}

int main(int args, char* argv[])
{
    LogInit();
    g_scheduler->Start();
    // 解析命令行参数
    AssertWithInfo(args == 2, "please input config file!");

    
    ConfigInit(argv[1]);
    gateway::GatewayService::GetInstance().Start();
    g_scheduler->Stop();
    return 0;
}