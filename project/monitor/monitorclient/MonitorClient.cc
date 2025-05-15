#include <bbt/pollevent/Event.hpp>
#include <monitor/monitorclient/MonitorClient.hpp>

using namespace bbt::core::errcode;
using namespace service::protocol;

namespace service::monitor
{

MonitorClient::MonitorClient(std::shared_ptr<bbt::pollevent::EvThread> thread, const std::string& service_name, const std::string& ip, short port):
    bbt::rpc::RpcClient(thread),
    m_thread(thread),
    m_service_name(service_name),
    m_service_ip(ip),
    m_service_port(port)
{
}

bbt::core::errcode::ErrOpt MonitorClient::RunInEvThread()
{
    auto& config = MonitorClientConfig::GetInstance();

    m_feed_dog_interval_ms = config->m_feed_dog_interval;

    if (auto err = Init(config->m_ip.c_str(), config->m_port, config->m_connect_timeout, config->m_client_timeout); err.has_value())
    {
        BBT_FULL_LOG_ERROR("Failed to init monitor client: %s", err->CWhat());
        return err;
    }

    BBT_BASE_LOG_INFO("[MonitorClient] initialized successfully! %s:%d", config->m_ip.c_str(), config->m_port);
    return std::nullopt;
}

void MonitorClient::Update()
{
    if (!IsConnected()) {
        if (bbt::core::clock::is_expired<bbt::core::clock::ms>(m_last_connect_ts + bbt::core::clock::ms(m_try_connect_interval))) {
            m_last_connect_ts = bbt::core::clock::now();
            ReConnect();
        }
        return;
    }

    { // watch dog
        if (bbt::core::clock::is_expired<bbt::core::clock::ms>(m_last_feed_ts + bbt::core::clock::ms(m_feed_dog_interval_ms)))
        {
            m_last_feed_ts = bbt::core::clock::now();

            if (auto err = DoFeedDog(); err.has_value())
            {
                BBT_FULL_LOG_ERROR("Failed to feed dog: %s", err->CWhat());
            }
        }
    }

}

ErrOpt MonitorClient::DoFeedDog()
{
    anywithmonitor::FeedDogReq req = std::make_tuple(uuid.ToString(), m_service_name, m_service_ip, m_service_port);
    auto err = RemoteCallWithTuple("FeedDog", 1000, req, nullptr);
    return err;
}


ErrTuple<ServiceInfo> MonitorClient::GetServiceInfoCo(const std::string& service_name)
{
    ServiceInfo info;
    ErrOpt err;
    anywithmonitor::GetServiceInfoReq req = std::make_tuple(service_name);

    if (!g_bbt_tls_helper->EnableUseCo())
        return {Errcode("GetServiceInfoCo must run in coroutine", ERR_UNKNOWN), info};

    auto cond = bbtco_make_cocond();
    auto remote_call_err = RemoteCallWithTuple("getserviceinfo", 1000, req, [cond, &err, &info](ErrOpt e, const bbt::core::Buffer& data){
        if (e.has_value())
        {
            err = e;
            return;
        }

        anywithmonitor::GetServiceInfoResp args;
        err = bbt::core::codec::DeserializeWithTuple(data, args);

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

void MonitorClient::UpdateServiceInfo(const std::string& service_name, const std::string& ip, int port)
{
    m_service_name = service_name;
    m_service_ip = ip;
    m_service_port = port;
}

} // MonitorClient::MonitorClient()