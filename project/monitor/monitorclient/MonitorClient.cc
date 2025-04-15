#include <monitor/monitorclient/MonitorClient.hpp>
#include <bbt/pollevent/Event.hpp>

using namespace bbt::core::errcode;

namespace service::monitor
{

bbt::rpc::detail::RpcCodec codec;

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

ErrTuple<ServiceInfo> MonitorClient::GetServiceInfoCo(const std::string& service_name)
{
    ServiceInfo info;
    ErrOpt err;

    if (!g_bbt_tls_helper->EnableUseCo())
        return {Errcode("GetServiceInfoCo must run in coroutine", ERR_UNKNOWN), info};

    auto cond = bbtco_make_cocond();
    auto remote_call_err = RemoteCall("getserviceinfo", 1000, [cond, &err, &info](ErrOpt e, const bbt::core::Buffer& data){
        if (err.has_value())
        {
            err = e;
            return;
        }

        ServiceInfo info;

        auto [err, results] = codec.Deserialize(data);

        if (results.size() != 3)
        {
            err = Errcode("GetServiceInfoCo: results size not match", ERR_UNKNOWN);
            return;
        }

        info.service_name = results[0].string;
        info.ip = results[1].string;
        info.port = results[2].value.int32_value;
        cond->NotifyOne();

    }, service_name);

    if (remote_call_err.has_value())
    {
        return {err, info};
    }

    cond->Wait();
    return {err, info};
}

} // MonitorClient::MonitorClient()