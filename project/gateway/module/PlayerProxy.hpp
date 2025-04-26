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
#include <boost/noncopyable.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <bbt/network/TcpClient.hpp>

#include <gateway/Define.hpp>
#include <gateway/sceneclient/SceneClient.hpp>
#include <protocol/Define.hpp>

namespace service::gateway
{

/**
 * @brief 维护websocket，并转发消息到相应的服务
 * 作为内网服务的client，和外网的server
 */
class PlayerProxy:
    public std::enable_shared_from_this<PlayerProxy>,
    boost::noncopyable
{
public:
    PlayerProxy() = default;
    ~PlayerProxy() = default;

    ErrOpt Init(std::shared_ptr<bbt::pollevent::EvThread> io_thread, const std::string& host, int port);
    void Start();
    void Stop();

private:
    void OnUpdateCo();
    void OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);
    void OnOpen(websocketpp::connection_hdl hdl);
    void OnClose(websocketpp::connection_hdl hdl);
    void OnConnectionTimeout(const websocketpp::lib::error_code& ec, websocketpp::connection_hdl hdl);

    ErrOpt DoProxy(protocol::PlayerId id, const std::string& data);
private:
    std::mutex m_all_opt_mtx;

    std::shared_ptr<SceneClient> m_scene_client{nullptr};
    std::shared_ptr<websocketpp::server<websocketpp::config::asio>> m_server{nullptr};
    int m_websocket_timeout{5000}; // websocket连接超时时间

    std::unordered_map<protocol::PlayerId, websocketpp::connection_hdl> m_player_conn_map; // 玩家连接

    std::string m_ip;
    int m_port{0};
};

} // namespace service::gateway