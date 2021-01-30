#pragma once

#include <string>

namespace Engine
{
namespace Mono
{

enum class Access
{
    Private,
    ProtectedInternal,
    Internal,
    Protected,
    Public
};

inline std::string toString(Access acs)
{
    switch (acs)
    {
        case Access::Public:
            return "public";
        case Access::Private:
            return "private";
        case Access::Protected:
            return "protected";
        case Access::Internal:
            return "internal";
        case Access::ProtectedInternal:
            return "protected internal";
    };

    return "";
}

}
}