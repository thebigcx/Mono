#pragma once

#include <string>
#include <cassert>
#include <mono/jit/jit.h>

#include <Mono/String.h>

namespace Engine
{
namespace Mono
{

template<typename F, typename Tuple, std::size_t... I>
decltype(auto) applyImpl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
decltype(auto) apply(F&& f, Tuple&& t)
{
    using indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
    return applyImpl(std::forward<F>(f), std::forward<Tuple>(t), indices());
}

template<typename T>
struct ConvertMonoType
{
    static T toMono(const T& type)
    {
        return type;
    }

    static T fromMonoUnboxed(MonoObject* object)
    {
        return object;
    }

    static T fromMonoBoxed(MonoObject* object)
    {
        void* ptr = mono_object_unbox(object);
        return *reinterpret_cast<T*>(ptr);
    }
};

template<>
struct ConvertMonoType<Object>
{
    static MonoObject* toMono(const Object& obj)
    {
        return obj.get();
    }

    static Object fromMonoUnboxed(MonoObject* obj)
    {
        return Object(obj);
    }

    static Object fromMonoBoxed(MonoObject* obj)
    {
        return Object(obj);
    }
};

template<>
struct ConvertMonoType<std::string>
{
    static MonoObject* to_mono(const std::string& str)
    {
        auto domain = Domain::getCurrentDomain();
        return String(*domain, str).get();
    }

    static std::string from_mono_unboxed(MonoObject* obj)
    {
        return String(Object(obj)).asUtf8();
    }

    static std::string from_mono_boxed(MonoObject* obj)
    {
        return String(Object(obj)).asUtf8();
    }
};

}
}