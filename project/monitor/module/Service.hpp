#pragma once
#include <bbt/rpc/RpcServer.hpp>
#include <bbt/pollevent/EvThread.hpp>
#include <monitor/Define.hpp>
#include <monitor/monitorclient/MonitorProtocols.hpp>

namespace service::monitor
{

class Service
{
public:
    Service() = default;
    ~Service() = default;

    ErrOpt Init(std::shared_ptr<bbt::pollevent::EvThread> thread, const char* ip, short port, int connection_timeout = 10000);

    void OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    void OnGetServiceInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    void OnUpdate();
private:
    std::shared_ptr<bbt::rpc::RpcServer> m_rpc_server{nullptr};
    std::shared_ptr<bbt::pollevent::Event> m_update_event{nullptr};

    std::unordered_map<std::string, ServiceInfo> m_service_info_map;
};

} // namespace service::monitor