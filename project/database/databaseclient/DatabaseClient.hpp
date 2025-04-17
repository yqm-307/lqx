#pragma once
#include <bbt/rpc/RpcClient.hpp>
#include <bbt/pollevent/EvThread.hpp>
#include <bbt/pollevent/Event.hpp>

namespace service::database
{

class DatabaseClient:
    public bbt::rpc::RpcClient
{
public:
    DatabaseClient(bbt::pollevent::EvThread::SPtr thread, const std::string& service_name, const std::string& ip, short port);
    ~DatabaseClient() = default;

    ErrOpt RunInEvThread();
};

} // namespace service::database