#pragma once

#include <string>

#include <mono/jit/jit.h>

#include <Mono/Domain.h>
#include <Mono/Exception.h>
#include <Mono/TypeConversion.h>

namespace Mono
{

#define MONO_BIND_FN(func) [](auto... args) -> decltype(auto) { return func(std::move(args)...); }

static Domain* s_domain;

inline Domain& getCurrentDomain()
{
    return *s_domain;
}

template<typename T>
struct InternalConvertTypeToMono
{
    using Decay = typename std::decay<T>::type;

    using Result = typename std::conditional<
        CanBeTriviallyConverted<Decay>::value,
        const Decay*, decltype(ToMonoConverter<T>::convert(std::declval<MonoDomain*>(), std::declval<Decay>()))
    >::type;
};

template<>
struct InternalConvertTypeToMono<void>
{
    using Result = void;
};

template<typename T>
class InternalConvertTupleTypesToMono;

template<typename T>
class InternalConvertTupleTypesToMono<std::tuple<T>>
{
    using ResultType = typename InternalConvertTypeToMono<T>::Result;

public:
    using Result = std::tuple<ResultType>;
};

template<typename T, typename... Args>
class InternalConvertTupleTypesToMono<std::tuple<T, Args...>>
{
    template<typename... Args1, typename... Args2>
    static std::tuple<Args1..., Args2...> mergeTuples(std::tuple<Args1...>*, std::tuple<Args2...>*);

    using ResultHeadTuple = typename InternalConvertTupleTypesToMono<std::tuple<T>>::Result;
    using ResultTailTuple = typename InternalConvertTupleTypesToMono<std::tuple<Args...>>::Result;

public:
    using Result = decltype(mergeTuples((ResultHeadTuple*)nullptr, (ResultTailTuple*)nullptr));
};

template<>
class InternalConvertTupleTypesToMono<std::tuple<>>
{
public:
    using Result = std::tuple<>;
};

template<typename T>
struct InternalFunctionWrapper;

// Convert all types to mono types, allowing for nice wrapping of a C function to C#
template<typename R, typename... Args>
struct InternalFunctionWrapper<R(Args...)>
{
private:
    template<typename F, typename MonoReturnType, typename... MonoArgs>
    static typename std::enable_if<!std::is_void<MonoReturnType>::value, MonoReturnType>::type invokeInnerFunction(MonoArgs&&... args)
    {
        MonoDomain* domain = getCurrentDomain().get();
        std::aligned_storage<sizeof(F)> dummy;
        auto result = reinterpret_cast<F*>(&dummy)->operator()(FromMonoConverter<Args>::convert(domain, args)...);
        return ToMonoConverter<R>::convert(domain, std::move(result));
    }

    template<typename F, typename MonoReturnType, typename... MonoArgs>
    static typename std::enable_if<std::is_void<MonoReturnType>::value, void>::type invokeInnerFunction(MonoArgs&&... args)
    {
        MonoDomain* domain = getCurrentDomain().get();
        std::aligned_storage<sizeof(F)> dummy;
        reinterpret_cast<F*>(&dummy)->operator()(FromMonoConverter<Args>::convert(domain, args)...);
    }

    using ArgumentList = typename InternalConvertTupleTypesToMono<std::tuple<Args...>>::Result;
    using ReturnType = typename InternalConvertTypeToMono<R>::Result;

    template<typename F, typename MonoReturnType, typename... MonoArgs>
    static auto getImpl(MonoReturnType*, std::tuple<MonoArgs...>*)
    {
        using PureFuncType = MonoReturnType(*)(MonoArgs...);

        PureFuncType pureFunc = [](MonoArgs... args) -> MonoReturnType
        {
            return invokeInnerFunction<F, MonoReturnType, MonoArgs...>(std::forward<MonoArgs>(args)...);
        };

        return pureFunc;
    }

public:
    template<typename F>
    static auto get()
    {
        return getImpl<F>((ReturnType*)nullptr, (ArgumentList*)nullptr);
    }

private:

};


// Initialize the JIT and return the app domain for further use
inline Domain& init(const std::string& monoLib, const std::string& monoEtc, const std::string& appDomain)
{
    mono_set_dirs(monoLib.c_str(), monoEtc.c_str());

    auto jitDomain = mono_jit_init_version(appDomain.c_str(), "v4.0.30319");

    if (!jitDomain)
    {
        throw Exception("Mono JIT could not initialize.");
    }

    s_domain = new Domain(jitDomain);

    return *s_domain;
}

// Register an external function call in the assembly
template<typename Signature, typename F>
inline void addInternalCall(const std::string& signature, F&& func)
{
    auto wrapper = InternalFunctionWrapper<Signature>::template get<F>();
    mono_add_internal_call(signature.c_str(), reinterpret_cast<const void*>(wrapper));
}

// Shutdown the JIT
inline void shutdown()
{
    mono_jit_cleanup(s_domain->get());
    delete s_domain;
}

}