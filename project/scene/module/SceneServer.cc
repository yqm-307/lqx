#include <scene/module/SceneServer.hpp>
#include <bbt/pollevent/Event.hpp>
#include <protocol/Protocol.hpp>

#define REGISTER_METHOD(method_name, func) \
    if (auto err = RegisterMethod(method_name, [this](auto, bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data){ \
        return func(id, seq, data);}); err.has_value()) \
        return err;

using namespace service::protocol;

namespace service::scene
{

SceneServer::SceneServer(bbt::pollevent::EvThread::SPtr io_thread, const std::string& ip, int port, int connection_timeout):
    bbt::rpc::RpcServer(io_thread),
    m_ip(ip),
    m_port(port),
    m_connection_timeout(connection_timeout)
{
    m_update_event = io_thread->RegisterEvent(0, bbt::pollevent::EventOpt::PERSIST, [this](auto, auto, auto) { OnUpdate(); });
}

ErrOpt SceneServer::Init()
{
    if (auto err = bbt::rpc::RpcServer::Init(m_ip.c_str(), m_port, m_connection_timeout); err.has_value())
        return err;

    REGISTER_METHOD("addobject", OnAddObject);
    REGISTER_METHOD("removeobject", OnRemoveObject);
    REGISTER_METHOD("getallobjectinfo", OnGetAllObjectInfo);
    REGISTER_METHOD("gatewayproxy", OnProxyProtocol);

    m_update_event->StartListen(500);

    BBT_BASE_LOG_INFO("[SceneServer] started! %s:%d", m_ip.c_str(), m_port);

    return std::nullopt;
}

void SceneServer::OnUpdate()
{
}

ErrOpt SceneServer::OnAddObject(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    BBT_BASE_LOG_DEBUG("OnAddObject");
    return std::nullopt;
}

ErrOpt SceneServer::OnRemoveObject(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    BBT_BASE_LOG_DEBUG("OnRemoveObject");
    return std::nullopt;
}

ErrOpt SceneServer::OnGetAllObjectInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    BBT_BASE_LOG_DEBUG("OnGetAllObjectInfo");
    return std::nullopt;
}

ErrOpt SceneServer::OnProxyProtocol(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    // BBT_BASE_LOG_DEBUG("OnProxyProtocol [%s]", data.Peek());
    g2s::PlayerProtocolProxyRequest req;

    if (auto err = bbt::core::codec::DeserializeWithTuple(data, req); err.has_value())
        return err;
    
    BBT_BASE_LOG_INFO("[SceneServer] OnProxyProtocol [%d] %s", std::get<0>(req), std::get<1>(req).c_str());

    return std::nullopt;
}

} // namespace service::scene