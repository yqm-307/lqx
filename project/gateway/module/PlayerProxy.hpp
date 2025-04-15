/**
 * @file PlayerProxy.hpp
 * @author your name (you@domain.com)
 * @brief 玩家连接代理，用于处理玩家的连接和消息并转发到相应的服务
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include <gateway/Define.hpp>
#include <bbt/network/TcpClient.hpp>

namespace service::gateway
{

/**
 * @brief 维护websocket，并转发消息到相应的服务
 * 作为内网服务的client，和外网的server
 */
class PlayerProxy
{
public:
    PlayerProxy() = default;
    ~PlayerProxy() = default;

    void Init(bbt::network::ConnId player_conn, const std::string& service_name);

    bool IsConnected();
    ErrOpt TransmitMessage(const char* msg, size_t len);
    void KeepAlive();
private:
    bbt::network::ConnId m_client_conn_id{-1}; // 玩家连接
    std::shared_ptr<bbt::network::TcpClient> m_service_client{nullptr}; // 内网服务连接
};

} // namespace service::gateway