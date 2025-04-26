/**
 * @file SceneServer.hpp
 * @author your name (you@domain.com)
 * @brief 场景服务器
 * @version 0.1
 * @date 2025-04-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include <scene/module/Define.hpp>
#include <bbt/rpc/RpcServer.hpp>

namespace service::scene
{

class SceneServer:
    public bbt::rpc::RpcServer
{
public:
    SceneServer(bbt::pollevent::EvThread::SPtr io_thread, const std::string& ip, int port, int connection_timeout);
    virtual ~SceneServer() override = default;

    ErrOpt Init();

private:
    void OnUpdate();
    void OnAccept(bbt::network::ConnId connid, const bbt::network::IPAddress& addr) override { BBT_BASE_LOG_DEBUG("[SceneServer] OnAccept %d, %s", connid, addr.GetIPPort().c_str()); }
    void OnTimeout(bbt::network::ConnId connid) override { BBT_BASE_LOG_DEBUG("[SceneServer] OnTimeout %d", connid); }

    ErrOpt OnProxyProtocol(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    ErrOpt OnAddObject(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    ErrOpt OnRemoveObject(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    ErrOpt OnGetAllObjectInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
private:
    std::string m_ip;

    std::shared_ptr<bbt::pollevent::Event> m_update_event{nullptr};

    int m_port;
    int m_connection_timeout;
};

} // namespace service::scene