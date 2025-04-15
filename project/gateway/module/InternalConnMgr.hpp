#pragma once
#include <gateway/Define.hpp>
#include <bbt/network/TcpClient.hpp>

namespace service::gateway
{

/**
 * @brief 内网连接管理
 * 
 * 负责内网连接的管理，包括连接的创建、维护、数据收发和销毁等操作。
 */
class InternalConnMgr
{
public:
    InternalConnMgr() = default;
    ~InternalConnMgr() = default;
private:
};

} // namespace service::gateway