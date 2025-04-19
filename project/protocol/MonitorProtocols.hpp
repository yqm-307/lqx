#pragma once
#include <string>
#include <tuple>
#include <bbt/rpc/detail/Define.hpp>

namespace service::protocol
{

struct ServiceInfo
{
    std::string service_name;
    std::string ip;
    int port;
};

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

}