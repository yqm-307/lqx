#include <bbt/pollevent/EvThread.hpp>
#include <gateway/module/ArgsOptions.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <monitor/monitorclient/MonitorClientConfig.hpp>

using namespace service;

void LogInit()
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix("gateway");
}

int main(int args, char* argv[])
{
    LogInit();
    // 解析命令行参数
    gateway::ArgsOptions args_options(args, argv);
    if (auto err = args_options.parseCommandLine(); err.has_value())
    {
        BBT_FULL_LOG_ERROR("parse command line failed! %s", err->What().c_str());
        return -1;
    }

    // 加载配置文件
    std::string config_file = args_options.getConfigFile();
    BBT_FULL_LOG_INFO("config file=%s", config_file.c_str());

    // 解析 monitor 配置
    auto& monitor_config = monitor::MonitorClientConfig::GetInstance();
    monitor_config->LoadConfig(config_file);

    // 运行 monitor client
    auto thread = std::make_shared<bbt::pollevent::EvThread>();
    auto monitor_client = std::make_shared<monitor::MonitorClient>(thread, "gateway");
    if (auto err = monitor_client->RunInEvThread(monitor_config->m_ip.c_str(), monitor_config->m_port, monitor_config->m_connect_timeout, monitor_config->m_client_timeout); err.has_value())
    {
        BBT_FULL_LOG_ERROR("monitor client run in ev thread failed! %s", err->What().c_str());
        return -1;
    }

    BBT_FULL_LOG_INFO("monitor client run in ev thread success!");
    thread->Start();
    thread->Join();
    return 0;
}