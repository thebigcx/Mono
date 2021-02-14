#pragma once

#include <string>
#include <tuple>
#include <cassert>
#include <memory>
#include <mono/jit/jit.h>

#include <Mono/Exception.h>
#include <Mono/Object.h>
#include <Mono/TypeConversionForward.h>

namespace Mono
{

template<>
struct ToMonoConverter<std::string>
{
    static MonoString* convert(const Domain& domain, const std::string& str)
    {
        return mono_string_new(domain.get(), str.c_str());
    }
};

template<>
struct ToMonoConverter<const char*>
{
    static MonoString* convert(const Domain& domain, const char* str)
    {
        return mono_string_new(domain.get(), str);
    }
};

template<>
struct ToMonoConverter<char*>
{
    static MonoString* convert(const Domain& domain, const char* str)
    {
        return mono_string_new(domain.get(), str);
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

template<>
struct ToMonoConverter<Object>
{
    static MonoObject* convert(const Domain& domain, const Object& object)
    {
        return object.get();
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

    static std::string convert(const Domain& domain, const std::string* str)
    {
        return *str;
    }
};

template<>
struct FromMonoConverter<Object>
{
    static Object convert(const Domain& domain, MonoObject* object)
    {
        return Object(object);
    }
};

template<typename T>
struct CanBeTriviallyConverted
{
    //static constexpr size_t value = std::is_standard_layout<T>::value;
    static constexpr size_t value = false;
};

template<>
struct CanBeTriviallyConverted<Object>
{
    static constexpr size_t value = false;
};

}