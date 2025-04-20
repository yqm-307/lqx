#include <monitor/module/MonitorServer.hpp>
#include <monitor/module/MonitorManager.hpp>

namespace service::monitor
{

using namespace protocol;

MonitorServer::MonitorServer(std::shared_ptr<bbt::network::EvThread> io_thread, const char* ip, int port, int connection_timeout)
    : bbt::rpc::RpcServer(io_thread),
    m_io_thread(io_thread),
    m_ip(ip),
    m_port(port),
    m_connection_timeout(connection_timeout)
{
}

ErrOpt MonitorServer::Init()
{
    if (auto err = RpcServer::Init(m_ip.c_str(), m_port, m_connection_timeout); err.has_value())
        return err;

    if (auto err = RegisterMethod("FeedDog", 
    [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) { return OnFeedDog(id, seq, data); }); err.has_value())
        return err;
    
    if (auto err = RegisterMethod("getserviceinfo", 
    [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) { return OnGetServiceInfo(id, seq, data); }); err.has_value())
        return err;

    BBT_BASE_LOG_INFO("[MonitorServer] Init success, ip: %s port: %d", m_ip.c_str(), m_port);
    return std::nullopt;
}

ErrOpt MonitorServer::OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    anywithmonitor::FeedDogReq params;

    if (auto err = bbt::rpc::codec::DeserializeWithTuple(data, params); err.has_value())
        return err;

    MonitorManager::GetInstance()->Enliven(params);

    DoReply(id, seq, anywithmonitor::FeedDogResp{bbt::rpc::emRpcReplyType::RPC_REPLY_TYPE_SUCCESS, "feed succ!"});
    BBT_BASE_LOG_DEBUG("[Rpc OnFeedDog] uuid: %s service_name: %s ip: %s port: %d",
        std::get<0>(params).c_str(), std::get<1>(params).c_str(), std::get<2>(params).c_str(), std::get<3>(params));
    return std::nullopt;
}

ErrOpt MonitorServer::OnGetServiceInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    anywithmonitor::GetServiceInfoReq req;
    anywithmonitor::GetServiceInfoResp resp;
    if (auto err = bbt::rpc::codec::DeserializeWithTuple(data, req); err.has_value())
        return err;

    auto service_name = std::get<0>(req);

    auto info = MonitorManager::GetInstance()->GetServiceInfo(service_name);
    if (!info.has_value())
        return Errcode{"service not found", emErr::ERR_UNKNOWN};

    resp = std::make_tuple(bbt::rpc::emRpcReplyType::RPC_REPLY_TYPE_SUCCESS ,info->name, info->ip, info->port);
    DoReply(id, seq, resp);
    BBT_FULL_LOG_DEBUG("[Rpc OnGetServiceInfo] service_name: %s ip: %s port: %d", info->name.c_str(), info->ip.c_str(), info->port);
    return std::nullopt;
}


} // namespace service::monitor