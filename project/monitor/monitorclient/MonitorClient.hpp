#pragma once
#include <bbt/rpc/RpcClient.hpp>
#include <bbt/core/log/Logger.hpp>
#include <bbt/core/clock/Clock.hpp>
#include <bbt/core/crypto/Uuid.hpp>

#include <monitor/Define.hpp>
#include <monitor/monitorclient/MonitorProtocols.hpp>

namespace service::monitor
{

/**
 * @brief 和monitor服务进行通信的客户端
 * 
 */
class MonitorClient:
    public bbt::rpc::RpcClient
{
public:
    MonitorClient(std::shared_ptr<bbt::pollevent::EvThread> thread, const std::string& service_name="default");
    ~MonitorClient() = default;

    ErrOpt RunInEvThread(
        const char* ip, int port,
        int connect_timeout=1000, int connection_timeout=5000, int feed_dog_interval=2000);

    void OnError(const bbt::core::errcode::Errcode& err) override
    {
        BBT_FULL_LOG_ERROR("[MonitorClient] %s", err.What().c_str());
    }

    /**
     * @brief 获取一个service的信息
     * 
     * @return bbt::core::errcode::ErrTuple<ServiceInfo> 
     */
    ErrTuple<ServiceInfo> GetServiceInfoCo(const std::string& service_name);

private:
    void OnUpdate();
private:
    bbt::core::crypto::Uuid uuid;
    std::string m_service_name{""};
    std::shared_ptr<bbt::pollevent::EvThread> m_thread{nullptr};
    // std::shared_ptr<bbt::rpc::RpcClient> m_client{nullptr};
    std::shared_ptr<bbt::pollevent::Event> m_update_event{nullptr};
    int m_feed_dog_interval_ms{2000}; // 喂狗的时间间隔，默认2s
    bbt::core::clock::Timestamp<> m_last_feed_time{bbt::core::clock::now()};
};

} // namespace service::database