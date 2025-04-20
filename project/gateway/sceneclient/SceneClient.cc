#include <gateway/sceneclient/SceneClient.hpp>
#include <gateway/sceneclient/SceneClientConfig.hpp>
#include <protocol/Protocol.hpp>

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

    m_update_event = io_thread->RegisterEvent(0, bbt::pollevent::EventOpt::PERSIST, [this](auto, auto, auto) {
        OnUpdate();
    });
    
    return std::nullopt;
}

void SceneClient::OnUpdate()
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

ErrOpt SceneClient::ProxyProtocol(const std::string& data)
{
    g2s::PlayerProtocolProxyRequest req;
    req = std::make_tuple(data);

    return std::nullopt;
}



} // namespace service::scene