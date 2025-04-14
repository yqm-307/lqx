#include <monitor/monitorclient/MonitorClient.hpp>
#include <bbt/pollevent/Event.hpp>

using namespace bbt::core::errcode;

namespace service::monitor
{

MonitorClient::MonitorClient(std::shared_ptr<bbt::pollevent::EvThread> thread, const std::string& service_name):
    bbt::rpc::RpcClient(thread),
    m_thread(thread),
    m_service_name(service_name)
{
}

bbt::core::errcode::ErrOpt MonitorClient::RunInEvThread(
    const char* ip, int port,
    int connect_timeout,
    int connection_timeout,
    int feed_dog_interval)
{
    m_feed_dog_interval_ms = feed_dog_interval;

    if (auto err = Init(ip, port, connect_timeout, connection_timeout); err.has_value())
    {
        BBT_FULL_LOG_ERROR("Failed to init monitor client: %s", err->CWhat());
        return err;
    }

    if (m_update_event == nullptr)
    {
        m_update_event = m_thread->RegisterEvent(-1, bbt::pollevent::EventOpt::PERSIST, 
            [this](int fd, short event, auto) {
                OnUpdate();
            });
        
        m_update_event->StartListen(500);
    }

    return std::nullopt;
}

void MonitorClient::OnUpdate()
{
    if (!IsConnected()) {
        ReConnect();
        return;
    }

    { // watch dog
        if (bbt::core::clock::is_expired<bbt::core::clock::ms>(m_last_feed_time + bbt::core::clock::ms(m_feed_dog_interval_ms)))
        {
            m_last_feed_time = bbt::core::clock::now();

            auto err = RemoteCall("FeedDog", 1000, [this](ErrOpt err, const bbt::core::Buffer& buffer) {},
            // rpc call 参数
            uuid.ToString(), // 节点 uuid
            m_service_name // 节点名
            );
        
            if (err.has_value())
                BBT_FULL_LOG_ERROR("Failed to call FeedDog: %s", err->CWhat());
        }
    }

}

} // MonitorClient::MonitorClient()