#pragma once

#include <mono/jit/jit.h>

#include <Mono/Type.h>

namespace Engine
{
namespace Mono
{

class Object
{
public:
    Object() = default;
    Object(MonoObject* obj)
        : m_object(obj), m_type(Type(mono_object_get_class(obj))) {}

    const Type& getType() const { return m_type; }

    MonoObject* get() const { return m_object; }

protected:
    MonoObject* m_object = nullptr;
    Type m_type;
};

}
}