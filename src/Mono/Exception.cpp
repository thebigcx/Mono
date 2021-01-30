#include <Mono/Exception.h>
#include <Mono/Object.h>

namespace Engine
{
namespace Mono
{

ThunkException::ExceptionInfo getExceptionInfo(MonoObject* ex)
{
    auto obj = Object(ex);
    std::string type = obj.getType().getFullname();

    /*auto messageProperty = type.getProperty("Message");
    auto message = makePropertyInvoker<std::string>(messageProperty).getValue(obj);

    auto stacktraceProperty = type.getProperty("StackTrace");
    auto stacktrace = makePropertyInvoker<std::string>(stacktraceProperty).getValue(obj);*/
    std::string message = "";
    std::string stacktrace = "";

    return { type, message, stacktrace };
}

ThunkException::ThunkException(MonoObject* obj)
    : ThunkException(getExceptionInfo(obj))
{

}

ThunkException::ThunkException(const ExceptionInfo& inf)
    : Exception(inf.exceptionTypename + "(" + inf.message + ")\n" + inf.stacktrace), m_info(inf)
{

}

}
}