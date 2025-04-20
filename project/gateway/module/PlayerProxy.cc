#include <bbt/core/log/Logger.hpp>
#include <bbt/core/util/Assert.hpp>
#include <gateway/module/PlayerProxy.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
const bbt::network::ConnId CONNECTING_ID = -1; // 说明正在等待连接到内网服务

namespace service::gateway
{

ErrOpt PlayerProxy::Init(std::shared_ptr<bbt::pollevent::EvThread> io_thread, const std::string& host, int port)
{
    m_port = port;
    m_server = std::make_shared<server>();
    // m_http_server.set
    m_server->set_reuse_addr(true);
    m_server->init_asio();

    AssertWithInfo(shared_from_this() != nullptr, "must is shared_ptr");

    m_server->set_message_handler([weak_this{weak_from_this()}]
    (websocketpp::connection_hdl hdl, server::message_ptr msg)
    {
        auto pthis = weak_this.lock();
        if (!pthis)
            return;

        pthis->OnMessage(hdl, msg);
    });

    m_server->set_open_handler([weak_this{weak_from_this()}]
    (websocketpp::connection_hdl hdl)
    {
        auto pthis = weak_this.lock();
        if (!pthis)
            return;

        pthis->OnOpen(hdl);
    });

    m_server->set_close_handler([weak_this{weak_from_this()}]
    (websocketpp::connection_hdl hdl)
    {
        auto pthis = weak_this.lock();
        if (!pthis)
            return;

        pthis->OnClose(hdl);
    });

    m_scene_client = std::make_shared<SceneClient>(io_thread);

    if (auto err = m_scene_client->Init(io_thread); err.has_value())
        return err;


    return std::nullopt;
}

void PlayerProxy::Start()
{
    m_server->listen(m_port);

    m_server->start_accept();

    auto thread = std::thread([server{m_server}]()
    {
        server->run();
    });

    thread.detach();
    BBT_BASE_LOG_INFO("PlayerProxy started on %s:%d", m_ip.c_str(), m_port);
}

void PlayerProxy::Stop()
{
    m_server->stop();
}


void PlayerProxy::OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
    auto conn = m_server->get_con_from_hdl(hdl);
    auto endpoint = conn->get_remote_endpoint();
    auto addr = endpoint.c_str();

    conn->set_timer(m_websocket_timeout, [this, hdl](websocketpp::lib::error_code const &ec)
    { 
        if (ec)
        {
            BBT_BASE_LOG_ERROR("[PlayerProxy] %s", ec.message().c_str());
            return;
        }

        m_server->close(hdl, websocketpp::close::status::going_away, "timeout");
    });

    auto& data = msg->get_payload();
    if (auto err = m_scene_client->ProxyProtocol(data); err.has_value())
    {
        BBT_BASE_LOG_ERROR("[PlayerProxy] %s", err->CWhat());
        return;
    }

    BBT_BASE_LOG_DEBUG("[PlayerProxy] %s", data.c_str());
}

void PlayerProxy::OnOpen(websocketpp::connection_hdl hdl)
{
    try
    {
        auto conn = m_server->get_con_from_hdl(hdl);
        conn->set_timer(m_websocket_timeout, [this, hdl](websocketpp::lib::error_code const &ec)
        {
            if (ec)
            {
                BBT_BASE_LOG_ERROR("[PlayerProxy] %s", ec.message().c_str());
                return;
            }
            m_server->close(hdl, websocketpp::close::status::going_away, "timeout");
        });
    
        BBT_BASE_LOG_INFO("[PlayerProxy] new connection from %s, uri=%s", conn->get_remote_endpoint().c_str(), conn->get_uri()->str().c_str());
    }
    catch(const std::exception& e)
    {
        BBT_BASE_LOG_ERROR("[PlayerProxy] %s", e.what());
    }
    
}

void PlayerProxy::OnClose(websocketpp::connection_hdl hdl)
{
    try
    {
        auto conn = m_server->get_con_from_hdl(hdl);
        auto endpoint = conn->get_remote_endpoint();
        auto addr = endpoint.c_str();
    
        BBT_BASE_LOG_INFO("[PlayerProxy] connection closed from %s", addr);
    }
    catch(const std::exception& e)
    {
        BBT_BASE_LOG_ERROR("[PlayerProxy] %s", e.what());
    }
}

void PlayerProxy::OnUpdateCo()
{
    // 定时检查玩家的连接状态
    // while (true)
    // {
    //     std::this_thread::sleep_for(std::chrono::seconds(1));

    //     std::lock_guard<std::mutex> lock(m_all_opt_mtx);
    //     for (auto it = m_player_last_proxymsg_ts_map.begin(); it != m_player_last_proxymsg_ts_map.end();)
    //     {
    //         if (time(nullptr) - it->second > 10) // 超过10秒没有消息，断开连接
    //         {
    //             m_server->close(it->first, websocketpp::close::status::going_away, "timeout");
    //             it = m_player_last_proxymsg_ts_map.erase(it);
    //         }
    //         else
    //         {
    //             ++it;
    //         }
    //     }
    // }
}


}