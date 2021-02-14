#pragma once

#include <map>
#include <functional>

namespace Mono
{

template<typename Function>
struct FunctionTraits;

template<typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)> : public FunctionTraits<R(Args...)>
{

};

template<typename R, typename... Args>
struct FunctionTraits<R(Args...)>
{
    using ReturnType = R;
    using FunctionType = R(Args...);

    using Type = std::function<FunctionType>;
    
    static constexpr size_t arity = sizeof...(Args);

    using Arguments = std::tuple<Args...>;

    template<std::size_t N>
    struct Argument
    {
        static_assert(N < arity, "Invalid parameter index.");
        using Type = typename std::tuple_element<N, Arguments>::type;
    };
};

}