#pragma once
#include <bbt/rpc/RpcServer.hpp>
#include <bbt/pollevent/EvThread.hpp>
#include <monitor/Define.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <monitor/monitorclient/MonitorProtocols.hpp>

namespace service::monitor
{

class MonitorService
{
public:
    static std::unique_ptr<MonitorService>& GetInstance();
    ~MonitorService() = default;

    void Start();
    void Stop();

    ErrOpt OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    ErrOpt OnGetServiceInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
private:
    MonitorService() = default;
    void InitRpcServer();
    void OnUpdateCo();
    void WaitToExit();

private:
    volatile bool m_is_running{true};
    std::shared_ptr<bbt::pollevent::EvThread> m_client_thread{nullptr};

    std::shared_ptr<bbt::pollevent::Event> m_signal_event{nullptr};
    std::shared_ptr<bbt::pollevent::Event> m_sigquit_event{nullptr};


    std::shared_ptr<bbt::rpc::RpcServer> m_monitor_server{nullptr};
    std::unordered_map<std::string, ServiceInfo> m_service_info_map;
};

} // namespace service::monitor