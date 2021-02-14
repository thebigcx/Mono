#pragma once

#include <mono/jit/jit.h>

namespace Mono
{

class Domain;

template<typename T>
struct ToMonoConverter
{
    static auto convert(const Domain& domain, const typename std::decay<T>::type& type)
    {
        return type;
    }
};

template<typename T>
struct FromMonoConverter
{
    // If the type needs to be unboxed from mono-types
    template<typename U>
    static typename std::enable_if<std::is_same<U, MonoObject*>::value, T>::type convert(const Domain& domain, U type)
    {
        return *reinterpret_cast<T*>(mono_object_unbox(type));
    }

    // POD types
    template<typename U>
    static typename std::enable_if<!std::is_same<U, MonoObject*>::value, T>::type convert(const Domain& domain, U type)
    {
        return type;
    }
};

}