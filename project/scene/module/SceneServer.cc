#include <scene/module/SceneServer.hpp>

#define REGISTER_METHOD(method_name, func) \
    if (auto err = RegisterMethod(#method_name, [this](auto, bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data){ \
        return func(id, seq, data);}); err.has_value()) \
        return err;

namespace service::scene
{

SceneServer::SceneServer(bbt::pollevent::EvThread::SPtr io_thread, const std::string& ip, int port, int connection_timeout):
    bbt::rpc::RpcServer(io_thread),
    m_ip(ip),
    m_port(port),
    m_connection_timeout(connection_timeout)
{

}

ErrOpt SceneServer::Init()
{
    if (auto err = bbt::rpc::RpcServer::Init(m_ip.c_str(), m_port, m_connection_timeout); err.has_value())
        return err;

    REGISTER_METHOD("addobject", OnAddObject);
    REGISTER_METHOD("removeobject", OnRemoveObject);
    REGISTER_METHOD("getallobjectinfo", OnGetAllObjectInfo);
    REGISTER_METHOD("gatewayproxy", OnProxyProtocol);

    return std::nullopt;
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
    BBT_BASE_LOG_DEBUG("OnProxyProtocol %s", data.Peek());
    return std::nullopt;
}

} // namespace service::scene