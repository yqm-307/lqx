#pragma once
#include <string>

namespace service::monitor
{

struct ServiceInfo
{
    std::string service_name;
    std::string ip;
    int port;
};

}