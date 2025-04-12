#pragma once
#include <bbt/core/log/Logger.hpp>
#include <bbt/core/errcode/Errcode.hpp>

namespace service::database
{

using namespace bbt::core::errcode;

enum emErr
{
    ERR_UNKNOWN = 1,
    ERR_PARSE_CMDLINE_FAILED,
};

// 一些配置项
#define FEED_DOG_INTERVAL_MS 2000 // 喂狗的时间间隔，默认2s


} // namespace service::database