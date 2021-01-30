#pragma once

#include <string>
#include <stdexcept>

#include <mono/jit/jit.h>

namespace Engine
{
namespace Mono
{

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what)
        : std::runtime_error(what) {}
};

class ThunkException : public Exception
{
public:
    ThunkException(MonoObject* obj);

    struct ExceptionInfo
    {
        std::string exceptionTypename;
        std::string message;
        std::string stacktrace;
    };

private:
    ThunkException(const ExceptionInfo& inf);

    ExceptionInfo m_info;
};

}
}