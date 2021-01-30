#include <Mono/Type.h>
#include <Mono/Domain.h>
#include <Mono/Object.h>
#include <Mono/Property.h>
#include <Mono/Field.h>
#include <Mono/Method.h>

namespace Engine
{
namespace Mono
{

Type::Type(MonoClass* c)
    : m_class(c)
{
    generateMeta();
}

Type::Type(MonoType* t)
{
    m_class = mono_class_from_mono_type(t);
    
    generateMeta();
}

Object Type::createInstance()
{
    Object obj(mono_object_new(Domain::getCurrentDomain()->get(), m_class));
    mono_runtime_object_init(obj.get());
    return obj;
}

Method Type::getMethod(const std::string& name)
{
    return Method(*this, name);
}

Field Type::getField(const std::string& name)
{
    return Field(*this, name);
}

Property Type::getProperty(const std::string& name) const
{
    return Property(*this, name);
}

std::vector<Field> Type::getFields()
{

}

std::vector<Property> Type::getProperties()
{

}

bool Type::hasBaseType()
{
    return mono_class_get_parent(m_class) != nullptr;
}

Type Type::getBaseType()
{
    auto base = mono_class_get_parent(m_class);
    return Type(base);
}

bool Type::isDerivedFrom(const Type& type)
{
    return mono_class_is_subclass_of(m_class, type.get(), false) != 0;
}

void Type::generateMeta()
{
    m_type = mono_class_get_type(m_class);
    m_namespace = mono_class_get_namespace(m_class);
    m_name = mono_class_get_name(m_class);
    m_fullname = m_namespace.empty() ? m_name : m_namespace + "." + m_name;
    m_rank = mono_class_get_rank(m_class);
    m_valueType = !!mono_class_is_valuetype(m_class);

    uint32_t align;
    m_sizeof = std::size_t(mono_class_value_size(m_class, &align));
    m_alignof = static_cast<std::size_t>(align);
}

}
}