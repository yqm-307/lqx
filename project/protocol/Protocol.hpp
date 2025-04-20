#pragma once
#include <protocol/Define.hpp>
#include <bbt/rpc/detail/Define.hpp>

namespace service::protocol
{

/**
 * @brief monitor相关的协议定义
 * 
 */
namespace anywithmonitor
{

typedef bbt::rpc::RemoteCallTemplateRequest<
    std::string,    // uuid
    std::string,    // service_name
    std::string,    // ip
    short           // port
> FeedDogReq;

typedef bbt::rpc::RemoteCallTemplateReply<
    std::string     // result
> FeedDogResp;

typedef bbt::rpc::RemoteCallTemplateRequest
<
    std::string     // service_name
> GetServiceInfoReq;

typedef bbt::rpc::RemoteCallTemplateReply
<
    std::string,    // service_name
    std::string,    // ip
    int             // port
> GetServiceInfoResp;

} // namespace anywithmonitor


/**
 * @brief gateway -> scene 协议定义
 * 
 */
namespace g2s
{

typedef bbt::rpc::RemoteCallTemplateRequest<
    std::string // 字节流
> PlayerProtocolProxyRequest;

} // namespace g2s


/**
 * @brief scene -> gateway 协议定义
 * 
 */
namespace s2g
{

} // namespace s2g

}