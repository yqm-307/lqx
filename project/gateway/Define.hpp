#pragma once
#include <bbt/core/errcode/Errcode.hpp>

namespace service::gateway
{

using namespace bbt::core::errcode;

enum emErr
{
    ERR_UNKNOWN = 1,
    ERR_PARSE_CMDLINE_FAILED,
};

} // namespace service::gateway