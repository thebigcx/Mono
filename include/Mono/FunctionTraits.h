#pragma once

#include <functional>

namespace Mono
{

template<typename T>
struct InternalGetFunctionTraits;

template<typename R, typename... Args>
struct InternalGetFunctionTraits<R(Args...)>
{
    using Type = std::function<R(Args...)>;
    using ResultType = R;

    static constexpr size_t ArgumentCount = sizeof...(Args);
};

}