#pragma once
#include <database/Define.hpp>
#include <bbt/rpc/RpcClient.hpp>
#include <bbt/core/clock/Clock.hpp>
#include <bbt/core/crypto/Uuid.hpp>

namespace service::database
{

/**
 * @brief 和monitor服务进行通信的客户端
 * 
 */
class MonitorClient:
    public bbt::rpc::RpcClient
{
public:
    MonitorClient(std::shared_ptr<bbt::pollevent::EvThread> thread);
    ~MonitorClient() = default;

    bbt::core::errcode::ErrOpt RunInEvThread(
        const char* ip, int port,
        int connect_timeout=1000, int connection_timeout=5000);

    // bbt::core::errcode::ErrOpt ReConnect();
    void OnError(const bbt::core::errcode::Errcode& err) override
    {
        BBT_FULL_LOG_ERROR("[MonitorClient] %s", err.What().c_str());
    }

private:
    void OnUpdate();
private:
    bbt::core::crypto::Uuid uuid;
    std::shared_ptr<bbt::pollevent::EvThread> m_thread{nullptr};
    // std::shared_ptr<bbt::rpc::RpcClient> m_client{nullptr};
    std::shared_ptr<bbt::pollevent::Event> m_update_event{nullptr};
    const int m_feed_dog_interval_ms{FEED_DOG_INTERVAL_MS}; // 喂狗的时间间隔，默认2s
    bbt::core::clock::Timestamp<> m_last_feed_time{bbt::core::clock::now()};
};

} // namespace service::database