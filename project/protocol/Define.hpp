#pragma once
#include <string>
#include <tuple>
#include <bbt/rpc/detail/Define.hpp>



namespace service::protocol
{

typedef int64_t PlayerId; // 玩家ID

// 定义协议中使用的结构体
#pragma pack(push, 1)

struct ExampleStruct
{
    int32_t id;
    char name[32];
};

/**
 * @brief 从monitor查询服务信息
 * 
 */
struct ServiceInfo
{
    std::string service_name;
    std::string ip;
    int port;
};


#pragma pack(pop)

} // namespace service::protocol
