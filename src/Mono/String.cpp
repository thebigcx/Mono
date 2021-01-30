#include <Mono/String.h>
#include <Mono/Domain.h>

#include <mono/jit/jit.h>

namespace Engine
{
namespace Mono
{

String::String(const Object& obj)
    : Object(obj)
{
    m_string = mono_object_to_string(m_object, nullptr);
}

String::String(const Domain& dom, const std::string& str)
    : Object(reinterpret_cast<MonoObject*>(mono_string_new(dom.get(), str.c_str())))
{
    m_string = mono_object_to_string(m_object, nullptr);
}

std::string String::asUtf8() const
{
    auto raw = mono_string_to_utf8(m_string);
    std::string str(reinterpret_cast<std::string::value_type*>(raw));
    mono_free(raw);
    return str;
}

std::u16string String::asUtf16() const
{
    auto raw = mono_string_to_utf16(m_string);
    std::u16string str(reinterpret_cast<std::u16string::value_type*>(raw));
    mono_free(raw);
    return str;
}

std::u32string String::asUtf32() const
{
    auto raw = mono_string_to_utf32(m_string);
    std::u32string str(reinterpret_cast<std::u32string::value_type*>(raw));
    mono_free(raw);
    return str;
}

}
}