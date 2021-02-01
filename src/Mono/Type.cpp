#include <Mono/Type.h>
#include <Mono/Domain.h>
#include <Mono/Object.h>
#include <Mono/Field.h>
#include <Mono/Method.h>
#include <Mono/Mono.h>

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

Type::Type(MonoImage* image, const std::string& nameSpace, const std::string& name)
{
    m_class = mono_class_from_name(image, nameSpace.c_str(), name.c_str());
}

Method Type::getMethod(const std::string& name)
{
    return Method(getCurrentDomain(), *this, name);
}

MonoClassField* Type::getField(const std::string& name)
{
    MonoClassField* field = mono_class_get_field_from_name(m_class, name.c_str());
    if (field == nullptr)
    {
        throw Exception("Could not find field: " + name + " in class: " + m_name);
    }

    return field;
}

std::vector<std::string> Type::getProperties()
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
