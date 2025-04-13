#pragma once
#include <bbt/core/log/Logger.hpp>
#include <bbt/core/errcode/Errcode.hpp>
#include <bbt/core/crypto/Uuid.hpp>

namespace service::monitor
{

using namespace bbt::core;
using namespace bbt::core::errcode;

enum emErr
{
    ERR_UNKNOWN = 0,
    ERR_PARSE_CMDLINE_FAILED,
};

class MonitorManager;

}