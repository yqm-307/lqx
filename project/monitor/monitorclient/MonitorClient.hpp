#pragma once
#include <bbt/rpc/RpcClient.hpp>
#include <bbt/core/log/Logger.hpp>
#include <bbt/core/clock/Clock.hpp>
#include <bbt/core/crypto/Uuid.hpp>

#include <monitor/Define.hpp>
#include <monitor/monitorclient/MonitorClientConfig.hpp>
#include <protocol/Protocol.hpp>

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
    MonitorClient(std::shared_ptr<bbt::pollevent::EvThread> thread, const std::string& service_name, const std::string& ip, short port);
    ~MonitorClient() = default;

    ErrOpt RunInEvThread();

    void OnError(const bbt::core::errcode::Errcode& err) override
    {
        BBT_FULL_LOG_ERROR("[MonitorClient] %s", err.What().c_str());
    }

    /**
     * @brief 获取一个service的信息
     * 
     * @return bbt::core::errcode::ErrTuple<ServiceInfo> 
     */
    ErrTuple<protocol::ServiceInfo> GetServiceInfoCo(const std::string& service_name);

    void Update();
    
    ErrOpt DoFeedDog();
    void UpdateServiceInfo(const std::string& service_name, const std::string& ip, int port);
private:
private:
    bbt::core::crypto::Uuid uuid;
    std::string m_service_name{""};
    std::string m_service_ip{""};
    short m_service_port{0};

    std::shared_ptr<bbt::pollevent::EvThread> m_thread{nullptr};
    int m_feed_dog_interval_ms{2000}; // 喂狗的时间间隔，默认2s
    bbt::core::clock::Timestamp<> m_last_feed_ts{bbt::core::clock::now()};

    int m_try_connect_interval{2000}; // 连接失败后，尝试连接的时间间隔
    bbt::core::clock::Timestamp<> m_last_connect_ts{bbt::core::clock::now()};

};

} // namespace service::database