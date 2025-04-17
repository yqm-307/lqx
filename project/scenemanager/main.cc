#include <monitor/monitorclient/MonitorClient.hpp>
#include <monitor/monitorclient/MonitorClientConfig.hpp>

void LogInit()
{
    bbt::core::log::Logger::GetInstance()->Stdout(true);
    bbt::core::log::Logger::GetInstance()->SetPrefix("gateway");
}

int main(int argc, char** argv)
{
    LogInit();
    
}