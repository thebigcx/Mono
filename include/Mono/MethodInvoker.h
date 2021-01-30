#pragma once

#include <vector>

#include <Mono/Method.h>
#include <Mono/Object.h>
#include <Mono/Exception.h>
#include <Mono/String.h>
#include <Mono/TypeConversion.h>
#include <Mono/TypeTraits.h>

namespace Engine
{
namespace Mono
{

template<typename T>
inline auto toMonoArg(T& arg)
{
    return std::addressof(arg);
}
inline auto toMonoArg(MonoObject* arg)
{
    return arg;
}

template<typename Signature>
bool hasCompatibleSignature(const Method& method)
{
    constexpr auto arity = FunctionTraits<Signature>::arity;
    using ReturnType = typename FunctionTraits<Signature>::ReturnType;
    using Arguments = typename FunctionTraits<Signature>::Arguments;

    auto expectedReturnType = method.getReturnType();
    auto expectedArguments = method.getParamTypes();

    bool compatible = arity == expectedArguments.size();

    return compatible;
}

template<typename T>
class MethodInvoker;

template<typename... Args>
class MethodInvoker<void(Args...)> : public Method
{
public:
    void operator()(Args&&... args)
    {
        invoke(nullptr, std::forward<Args>(args)...);
    }

    void operator()(const Object& obj, Args&&... args)
    {
        invoke(&obj, std::forward<Args>(args)...);
    }

private:
    void invoke(const Object* obj, Args&&... args)
    {
        auto method = m_method;

        MonoObject* monoObject = nullptr;
        if (obj)
        {
            monoObject = obj->get();

            method = mono_object_get_virtual_method(monoObject, method);
        }

        auto tuple = std::make_tuple(ConvertMonoType<std::decay_t<Args>>::toMono(std::forward<Args>(args))...);

        auto inv = [method, monoObject](auto... args)
        {
            std::vector<void*> monoArgs = { toMonoArg(args)... };

            MonoObject* ex = nullptr;
            MonoObject* result = mono_runtime_invoke(method, monoObject, monoArgs.data(), &ex);
            if (ex)
            {
                throw ThunkException(ex);
            }
        };

        Mono::apply(inv, tuple);
    }

    template<typename Signature>
    friend MethodInvoker<Signature> makeMethodInvoker(const Method& m);

    MethodInvoker(const Method& method)
        : Method(method) {}
};

template<typename Ret, typename... Args>
class MethodInvoker<Ret(Args...)> : public Method
{
public:
    Ret operator()(Args&&... args)
    {
        return invoke(nullptr, std::forward<Args>(args)...);
    }

    Ret operator()(const Object& obj, Args&&... args)
    {
        return invoke(&obj, std::forward<Args>(args)...);
    }

private:
    auto invoke(const Object* obj, Args&&... args)
    {
        auto method = m_method;

        MonoObject* monoObject = nullptr;
        if (obj)
        {
            monoObject = obj->get();

            method = mono_object_get_virtual_method(monoObject, method);
        }

        auto tuple = std::make_tuple(ConvertMonoType<std::decay_t<Args>>::toMono(std::forward<Args>(args))...);

        auto inv = [method, monoObject](auto... args)
        {
            std::vector<void*> monoArgs = { toMonoArg(args)... };

            MonoObject* ex = nullptr;
            MonoObject* result = mono_runtime_invoke(method, monoObject, monoArgs.data(), &ex);
            if (ex)
            {
                throw ThunkException(ex);
            }

            return result;
        };

        auto result = Mono::apply(inv, tuple);
        return ConvertMonoType<std::decay_t<Ret>>::fromMonoBoxed(std::move(result));
    }

    template<typename Signature>
    friend MethodInvoker<Signature> makeMethodInvoker(const Method& method);

    MethodInvoker(const Method& method)
        : Method(method) {}
};


template<typename Signature>
MethodInvoker<Signature> makeMethodInvoker(const Method& method)
{
    return MethodInvoker<Signature>(method);
}

}
}