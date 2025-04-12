#include <iostream>
#include <monitor/module/ArgsOptions.hpp>
#include <monitor/module/Service.hpp>
#include <monitor/module/Config.hpp>

using namespace service;

int main(int argc, char* argv[])
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    monitor::ArgsOptions args(argc, argv);
    if (auto err = args.parseCommandLine(); err.has_value())
    {
        BBT_FULL_LOG_ERROR("[monitor] parse command line failed, err=%s", err->CWhat());
        return -1;
    }


    auto& config = monitor::Config::GetInstance();
    config->LoadConfig(args.getConfigFile());
    BBT_FULL_LOG_INFO("[monitor] config file=%s", args.getConfigFile().c_str());
    BBT_FULL_LOG_INFO("[monitor] ip=%s, port=%d, timeout=%d", config->m_ip.c_str(), config->m_port, config->m_connection_timeout);

    auto thread = std::make_shared<bbt::pollevent::EvThread>();
    auto service = std::make_shared<monitor::Service>();

    if (auto err = service->Init(thread, config->m_ip.c_str(), config->m_port, config->m_connection_timeout); err.has_value())
    {
        BBT_FULL_LOG_ERROR("[monitor] service init failed, err=%s", err->CWhat());
        return -1;
    }

    BBT_FULL_LOG_INFO("[monitor] service started, ip=%s, port=%d", config->m_ip.c_str(), config->m_port);
    // 启动线程
    thread->Start();
    thread->Join();

    return 0;
}