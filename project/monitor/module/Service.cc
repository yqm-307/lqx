#include <monitor/module/Service.hpp>
#include <bbt/pollevent/Event.hpp>
#include <monitor/module/MonitorManager.hpp>

namespace service::monitor
{

ErrOpt Service::Init(std::shared_ptr<bbt::pollevent::EvThread> thread, const char* ip, short port, int connection_timeout)
{
    if (m_rpc_server == nullptr)
    {
        m_rpc_server = std::make_shared<bbt::rpc::RpcServer>(thread);
        m_rpc_server->Init(ip, port, connection_timeout);
    }

    // 注册方法

    m_rpc_server->RegisterMethod("FeedDog", 
        [this](auto server, auto id, auto seq, const bbt::core::Buffer& data) {
            // 处理 FeedDog 请求
            OnFeedDog(id, seq, data);
        });

    if (m_update_event == nullptr)
    {
        m_update_event = thread->RegisterEvent(-1, bbt::pollevent::EventOpt::PERSIST, [&](auto, auto, auto){
            OnUpdate();
        });
        m_update_event->StartListen(5000); // 1s
    }
    
    return std::nullopt;
}

void Service::OnFeedDog(bbt::network::ConnId id, bbt::rpc::RemoteCallSeq seq, const bbt::core::Buffer& data)
{
    bbt::rpc::detail::RpcCodec codec;

    auto [err, results] = codec.Deserialize(data);
    if (err.has_value())
    {
        // 处理错误
        BBT_FULL_LOG_ERROR("[Rpc OnFeedDog] %s", err->CWhat());
        return;
    }

    std::string result_msg;

    for (auto& result : results)
    {
        switch (result.header.field_type)
        {
        case bbt::rpc::detail::FieldType::INT64:
        case bbt::rpc::detail::FieldType::UINT32:
        case bbt::rpc::detail::FieldType::INT32:
        case bbt::rpc::detail::FieldType::UINT64:
            result_msg += std::to_string(result.value.uint64_value) + "\t";
            break;
        case bbt::rpc::detail::FieldType::STRING:
            result_msg += result.string + "\t";
            break;
        default:
            BBT_FULL_LOG_ERROR("unknown type %d", result.header.field_type);
        }
    }

    MonitorManager::GetInstance()->Enliven(results[0].string, results[1].string);
    m_rpc_server->DoReply(id, seq, result_msg);
}

void Service::OnUpdate()
{
    MonitorManager::GetInstance()->DebugPrint();
}


} // namespace service::monitor