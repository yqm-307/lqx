#include <bbt/pollevent/Event.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>
#include <monitor/monitorclient/MonitorProtocols.hpp>

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

            if (auto err = DoFeedDog(); err.has_value())
            {
                BBT_FULL_LOG_ERROR("Failed to feed dog: %s", err->CWhat());
            }
        }
    }

}

ErrOpt MonitorClient::DoFeedDog()
{
    FeedDogReq req = std::make_tuple(uuid.ToString(), m_service_name);
    auto err = RemoteCallWithTuple("FeedDog", 1000, req, nullptr);
    return err;
}


ErrTuple<ServiceInfo> MonitorClient::GetServiceInfoCo(const std::string& service_name)
{
    ServiceInfo info;
    ErrOpt err;
    GetServiceInfoReq req = std::make_tuple(service_name);

    if (!g_bbt_tls_helper->EnableUseCo())
        return {Errcode("GetServiceInfoCo must run in coroutine", ERR_UNKNOWN), info};

    auto cond = bbtco_make_cocond();
    auto remote_call_err = RemoteCallWithTuple("getserviceinfo", 1000, req, [cond, &err, &info](ErrOpt e, const bbt::core::Buffer& data){
        if (e.has_value())
        {
            err = e;
            return;
        }

        GetServiceInfoResp args;
        err = codec.DeserializeWithTuple(data, args);

        if (err.has_value())
            return;

        info.service_name = std::get<1>(args);
        info.ip = std::get<2>(args);
        info.port = std::get<3>(args);
        cond->NotifyOne();

    });

    if (remote_call_err.has_value())
    {
        return {err, info};
    }

    cond->Wait();
    return {err, info};
}

} // MonitorClient::MonitorClient()