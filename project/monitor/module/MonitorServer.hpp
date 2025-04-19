#pragma once
#include <bbt/rpc/RpcServer.hpp>
#include <bbt/pollevent/EvThread.hpp>
#include <monitor/Define.hpp>


namespace service::monitor
{

class MonitorServer:
    public bbt::rpc::RpcServer
{
public:
    MonitorServer(std::shared_ptr<bbt::network::EvThread> io_thread, const char* ip, int port, int connection_timeout);
    virtual ~MonitorServer() override = default;

    ErrOpt Init();
private:
    void OnError(const bbt::core::errcode::Errcode& err) override
    {
        BBT_BASE_LOG_ERROR("[MonitorServer] OnError: %s", err.CWhat());
    }

    void OnSend(bbt::network::ConnId connid, bbt::core::errcode::ErrOpt err, size_t send_len) override
    {
        if (err.has_value())
        {
            BBT_BASE_LOG_ERROR("[MonitorServer] OnSend: %s", err->CWhat());
        }

        BBT_BASE_LOG_DEBUG("[MonitorServer] OnSend: connid: %d, send_len: %zu", connid, send_len);
    }

    ErrOpt OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
    ErrOpt OnGetServiceInfo(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data);
private:
    std::shared_ptr<bbt::network::EvThread> m_io_thread{nullptr};
    std::string m_ip;
    short m_port;
    int m_connection_timeout;
};

} // namespace service::monitor