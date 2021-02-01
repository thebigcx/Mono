#pragma once

#include <string>
#include <tuple>
#include <cassert>
#include <memory>
#include <mono/jit/jit.h>

#include <Mono/Exception.h>

namespace Mono
{

class Object;

template<typename T>
struct ToMonoConverter
{
    static auto convert(const Domain& domain, const typename std::decay<T>::type& type)
    {
        return std::addressof(type);
    }
};

template<>
struct ToMonoConverter<std::string>
{
    static auto convert(const Domain& domain, const std::string& str)
    {
        return mono_string_new(domain.get(), str.c_str());
    }
};

template<>
struct ToMonoConverter<std::wstring>
{
    static auto convert(const Domain& domain, const std::wstring& str)
    {
        return mono_string_new_utf16(domain.get(), reinterpret_cast<const mono_unichar2*>(str.c_str()), static_cast<int32_t>(str.size()));
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

    // If type is already converted (no unboxing needed)
    template<typename U>
    static typename std::enable_if<!std::is_same<U, MonoObject*>::value, T>::type convert(const Domain& domain, U type)
    {
        return *type;
    }
};

template<>
struct FromMonoConverter<void>
{
    static void convert(const Domain& domain, MonoObject* t)
    {
        (void)t;
    }
};

template<>
struct FromMonoConverter<std::string>
{
    static std::string convert(const Domain& domain, MonoString* str)
    {
        std::string result;

        MonoError err;
        char* utf8Str = mono_string_to_utf8_checked(str, &err);

        if (err.error_code == MONO_ERROR_NONE)
        {
            result = utf8Str;
            mono_free(utf8Str);
        }

        return result;
    }

    static std::string convert(const Domain& domain, MonoObject* obj)
    {
        MonoObject* ex;
        MonoString* str = mono_object_to_string(obj, &ex);
        if (ex != nullptr)
        {
            throw Exception("Cannot convert Mono object to string.");
        }

        return convert(domain, str);
    }
};

template<typename T>
struct CanBeTriviallyConverted
{
    static constexpr size_t value = std::is_standard_layout<T>::value;
};

template<>
struct CanBeTriviallyConverted<Object>
{
    static constexpr size_t value = false;
};

}