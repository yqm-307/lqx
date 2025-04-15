#include <monitor/module/Service.hpp>
#include <bbt/pollevent/Event.hpp>
#include <monitor/module/MonitorManager.hpp>

namespace service::monitor
{

bbt::rpc::detail::RpcCodec codec;


ErrOpt Service::Init(std::shared_ptr<bbt::pollevent::EvThread> thread, const char* ip, short port, int connection_timeout)
{
    if (m_rpc_server == nullptr)
    {
        m_rpc_server = std::make_shared<bbt::rpc::RpcServer>(thread);
        m_rpc_server->Init(ip, port, connection_timeout);
    }

    // 注册方法

    m_rpc_server->RegisterMethod("FeedDog", 
        [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) {
            // 处理 FeedDog 请求
            OnFeedDog(id, seq, data);
        });
    
    m_rpc_server->RegisterMethod("getserviceinfo", 
        [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) {
            // 处理 GetServiceInfo 请求
            OnGetServiceInfo(id, seq, data);
        });

    if (m_update_event == nullptr)
    {
        m_update_event = thread->RegisterEvent(-1, bbt::pollevent::EventOpt::PERSIST, [&](auto, auto, auto){
            OnUpdate();
        });
        m_update_event->StartListen(5000); // 1s
    }
    
    return std::nullopt;
}

void Service::OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    bbt::rpc::detail::RpcCodec codec;

    std::tuple<std::string, std::string> params;

    if (auto err = codec.DeserializeWithArgs(data, params); err.has_value()) {
        m_rpc_server->DoReply(id, seq, err->CWhat());
        return;
    }

    m_service_info_map[std::get<1>(params)].ip = "0.0.0.0";
    m_service_info_map[std::get<1>(params)].port = 8080;
    m_service_info_map[std::get<1>(params)].service_name = std::get<1>(params);

    MonitorManager::GetInstance()->Enliven(std::get<0>(params), std::get<1>(params));
    m_rpc_server->DoReply(id, seq, "feed succ!");
}

void Service::OnGetServiceInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{

    std::tuple<std::string> params;
    if (auto err = codec.DeserializeWithArgs(data, params); err.has_value()) {
        m_rpc_server->DoReply(id, seq, err->CWhat());
        return;
    }

    auto service_name = std::get<0>(params);

    auto it = m_service_info_map.find(service_name);
    if (it == m_service_info_map.end())
    {
        BBT_FULL_LOG_ERROR("[Rpc OnGetServiceInfo] service not found");
        m_rpc_server->DoReply(id, seq, "service not found");
        return;
    }

    m_rpc_server->DoReply(id, seq, it->second.service_name, it->second.ip, it->second.port);
    BBT_FULL_LOG_DEBUG("[Rpc OnGetServiceInfo] service_name: %s ip: %s port: %d", it->second.service_name.c_str(), it->second.ip.c_str(), it->second.port);
}


void Service::OnUpdate()
{
    MonitorManager::GetInstance()->DebugPrint();
}


} // namespace service::monitor