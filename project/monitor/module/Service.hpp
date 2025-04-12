#pragma once
#include <monitor/Define.hpp>
#include <bbt/rpc/RpcServer.hpp>
#include <bbt/pollevent/EvThread.hpp>

namespace service::monitor
{

class Service
{
public:
    Service() = default;
    ~Service() = default;

    ErrOpt Init(std::shared_ptr<bbt::pollevent::EvThread> thread, const char* ip, short port, int connection_timeout = 10000);

    void OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
private:
    std::shared_ptr<bbt::rpc::RpcServer> m_rpc_server{nullptr};
};

} // namespace service::monitor