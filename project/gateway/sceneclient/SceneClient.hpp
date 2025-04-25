#pragma once
#include <gateway/Define.hpp>
#include <bbt/rpc/RpcClient.hpp>
#include <protocol/Protocol.hpp>

namespace service::gateway
{

class SceneClient:
    public bbt::rpc::RpcClient
{
public:
    SceneClient(std::shared_ptr<bbt::network::EvThread> io_thread);
    ~SceneClient() = default;

    ErrOpt Init(std::shared_ptr<bbt::pollevent::EvThread> io_thread);


    ErrOpt ProxyProtocol(protocol::PlayerId id, const std::string& data);

    void OnTimeout(bbt::network::ConnId id);
    void OnError(const bbt::core::errcode::Errcode& err);
private:
    void OnUpdate();
    ErrOpt OnProxyProtocol(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);

    std::string m_ip;
    int m_port{0};
    int m_try_connect_interval{2000}; // 连接失败后，尝试连接的时间间隔
    bbt::core::clock::Timestamp<> m_last_connect_ts{bbt::core::clock::now()};
    std::shared_ptr<bbt::pollevent::Event> m_update_event{nullptr};
};

} // namespace service::scene