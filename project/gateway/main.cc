#include <bbt/pollevent/EvThread.hpp>
#include <boost/process/environment.hpp>
#include <gateway/module/GatewayService.hpp>
#include <gateway/module/PlayerProxyConfig.hpp>

using namespace service;

void LogInit()
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix("gateway");
}

void ConfigInit(const std::string& config_file)
{
    g_bbt_coroutine_config->m_cfg_static_thread_num = 1;
    BBT_BASE_LOG_INFO("config file=%s", config_file.c_str());

    auto& config = monitor::MonitorClientConfig::GetInstance();
    config->LoadConfig(config_file);

    auto& config2 = gateway::PlayerProxyConfig::GetInstance();
    config2->LoadConfig(config_file);
}

int main(int args, char* argv[])
{
    LogInit();
    // 解析命令行参数
    AssertWithInfo(args == 2, "please input config file!");
    ConfigInit(argv[1]);

    // 开启服务
    if (auto err = gateway::GatewayService::GetInstance().Start(); err.has_value())
    {
        BBT_BASE_LOG_ERROR("GatewayService start failed: %s", err->What().c_str());
        return -1;
    }
    return 0;
}