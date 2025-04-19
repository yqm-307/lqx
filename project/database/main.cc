#include <bbt/core/log/Logger.hpp>
#include <bbt/pollevent/EvThread.hpp>
#include <database/module/DatabaseService.hpp>

using namespace service;


void LogInit()
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix("database");
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

    AssertWithInfo(args == 2, "please input config file!");
    ConfigInit(argv[1]);


    database::DatabaseService::GetInstance()->Start();
    return 0;
}