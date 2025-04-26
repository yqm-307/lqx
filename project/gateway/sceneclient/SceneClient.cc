#include <gateway/sceneclient/SceneClient.hpp>
#include <gateway/sceneclient/SceneClientConfig.hpp>
#include <protocol/Protocol.hpp>
#include <bbt/core/log/Logger.hpp>
#include <bbt/pollevent/Event.hpp>

using namespace service::protocol;

namespace service::gateway
{

SceneClient::SceneClient(std::shared_ptr<bbt::network::EvThread> io_thread):
    bbt::rpc::RpcClient(io_thread)
{
}


ErrOpt SceneClient::Init(std::shared_ptr<bbt::pollevent::EvThread> io_thread)
{
    auto& config = SceneClientConfig::GetInstance();

    if (auto err = bbt::rpc::RpcClient::Init(config->m_ip.c_str(), config->m_port, config->m_connect_timeout, config->m_client_timeout); err.has_value())
        return err;

    m_update_event = io_thread->RegisterEvent(0, bbt::pollevent::EventOpt::PERSIST, [this](auto, auto, auto) { Update(); });

    m_update_event->StartListen(500);

    BBT_BASE_LOG_INFO("[SceneClient] connect to %s:%d", config->m_ip.c_str(), config->m_port);
    
    return std::nullopt;
}

void SceneClient::Update()
{
    if (!IsConnected())
    {
        if (bbt::core::clock::is_expired<bbt::core::clock::ms>(m_last_connect_ts + bbt::core::clock::ms(m_try_connect_interval)))
        {
            m_last_connect_ts = bbt::core::clock::now();
            ReConnect();
        }
        return;
    }
}

ErrOpt SceneClient::ProxyProtocol(PlayerId id, const std::string& data)
{
    g2s::PlayerProtocolProxyRequest req = {id, data};

    return RemoteCallWithTuple("gatewayproxy", 1000, req, nullptr);
}

void SceneClient::OnTimeout(bbt::network::ConnId id)
{
    BBT_BASE_LOG_ERROR("[SceneClient] OnTimeout");
}

void SceneClient::OnError(const bbt::core::errcode::Errcode& err)
{
    BBT_BASE_LOG_ERROR("[SceneClient] OnError %s", err.What().c_str());
}


} // namespace service::scene