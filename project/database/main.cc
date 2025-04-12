#include <bbt/pollevent/EvThread.hpp>
#include <database/module/MonitorClient.hpp>
#include <database/module/Config.hpp>
#include <database/module/ArgsOptions.hpp>

using namespace service;

int main(int args, char* argv[])
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    // 解析命令行参数
    database::ArgsOptions args_options(args, argv);
    if (auto err = args_options.parseCommandLine(); err.has_value())
    {
        BBT_FULL_LOG_ERROR("parse command line failed! %s", err->What().c_str());
        return -1;
    }

    // 加载配置文件
    std::string config_file = args_options.getConfigFile();
    auto& config = database::Config::GetInstance();
    config->LoadConfig(config_file);

    BBT_FULL_LOG_INFO("config file=%s", config_file.c_str());


    auto thread = std::make_shared<bbt::pollevent::EvThread>();

    auto monitor_client = std::make_shared<database::MonitorClient>(thread);

    if (auto err = monitor_client->RunInEvThread(config->m_ip.c_str(), config->m_port, config->m_connect_timeout, config->m_client_timeout); err.has_value())
    {
        BBT_FULL_LOG_ERROR("monitor client run in ev thread failed! %s", err->What().c_str());
        return -1;
    }

    BBT_FULL_LOG_INFO("monitor client run in ev thread success!");
    thread->Start();
    thread->Join();
    return 0;
}