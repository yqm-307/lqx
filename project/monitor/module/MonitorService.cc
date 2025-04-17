#include <signal.h>
#include <bbt/pollevent/EvThread.hpp>
#include <bbt/pollevent/Event.hpp>
#include <monitor/module/MonitorManager.hpp>
#include <monitor/module/MonitorService.hpp>
#include <monitor/module/MonitorConfig.hpp>
#include <monitor/monitorclient/MonitorProtocols.hpp>

namespace service::monitor
{

using namespace bbt::pollevent;

std::unique_ptr<MonitorService>& MonitorService::GetInstance()
{
    static std::unique_ptr<MonitorService> instance(new MonitorService());
    return instance;
}

void MonitorService::InitRpcServer()
{
    auto& config = monitor::MonitorConfig::GetInstance();
    if (m_monitor_server == nullptr)
    {
        m_monitor_server = std::make_shared<bbt::rpc::RpcServer>(m_client_thread);
        m_monitor_server->Init(config->m_ip.c_str(), config->m_port, config->m_connection_timeout);
    }

    m_monitor_server->RegisterMethod("FeedDog", 
    [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) {
        // 处理 FeedDog 请求
        return OnFeedDog(id, seq, data);
    });
    
    m_monitor_server->RegisterMethod("getserviceinfo", 
    [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) {
        // 处理 GetServiceInfo 请求
        return OnGetServiceInfo(id, seq, data);
    });
}

void MonitorService::Start()
{
    g_scheduler->Start();

    m_client_thread = std::make_shared<bbt::pollevent::EvThread>();

    // 初始化 monitor server
    InitRpcServer();

    // 初始化信号监听
    m_signal_event = m_client_thread->RegisterEvent(SIGINT, EventOpt::SIGNAL | EventOpt::PERSIST, [this](auto, auto, auto){
        BBT_BASE_LOG_INFO("SIGINT received, exiting...");
        Stop();
    });

    m_signal_event->StartListen(0);

    m_sigquit_event = m_client_thread->RegisterEvent(SIGQUIT, EventOpt::SIGNAL | EventOpt::PERSIST, [this](auto, auto, auto){
        BBT_BASE_LOG_INFO("SIGQUIT received, exiting...");
        Stop();
    });

    m_sigquit_event->StartListen(0);
    m_client_thread->Start();

    bbtco [&](){ OnUpdateCo(); };

    WaitToExit();
}

void MonitorService::Stop()
{
    m_is_running = false;
}

ErrOpt MonitorService::OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    FeedDogReq params;

    if (auto err = bbt::rpc::codec::DeserializeWithTuple(data, params); err.has_value()) {
        return err;
    }

    auto& info = m_service_info_map[std::get<1>(params)];

    info.service_name = std::get<1>(params);
    info.ip = std::get<2>(params);
    info.port = std::get<3>(params);

    MonitorManager::GetInstance()->Enliven(std::get<0>(params), std::get<1>(params));

    m_monitor_server->DoReply(id, seq, FeedDogResp{bbt::rpc::emRpcReplyType::RPC_REPLY_TYPE_SUCCESS, "feed succ!"});
    return std::nullopt;
}

ErrOpt MonitorService::OnGetServiceInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    GetServiceInfoReq req;
    GetServiceInfoResp resp;
    if (auto err = bbt::rpc::codec::DeserializeWithTuple(data, req); err.has_value()) {
        return err;
    }

    auto service_name = std::get<0>(req);

    auto it = m_service_info_map.find(service_name);
    if (it == m_service_info_map.end())
    {
        BBT_FULL_LOG_ERROR("[Rpc OnGetServiceInfo] service not found");
        return Errcode{"service not found", emErr::ERR_UNKNOWN};
    }

    resp = std::make_tuple(bbt::rpc::emRpcReplyType::RPC_REPLY_TYPE_SUCCESS ,it->second.service_name, it->second.ip, it->second.port);
    m_monitor_server->DoReply(id, seq, resp);
    BBT_FULL_LOG_DEBUG("[Rpc OnGetServiceInfo] service_name: %s ip: %s port: %d", it->second.service_name.c_str(), it->second.ip.c_str(), it->second.port);
    return std::nullopt;
}


void MonitorService::OnUpdateCo()
{
    while (m_is_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        MonitorManager::GetInstance()->DebugPrint();
    }
}

void MonitorService::WaitToExit()
{
    while (m_is_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_client_thread->Stop();
    m_monitor_server = nullptr;
    g_scheduler->Stop();
}

} // namespace service::monitor