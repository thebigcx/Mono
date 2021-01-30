#include <Mono/Property.h>
#include <Mono/Exception.h>
#include <Mono/Method.h>

namespace Engine
{
namespace Mono
{

Property::Property(const Type& t, const std::string& name)
    : m_property(mono_class_get_property_from_name(t.get(), name.c_str())), m_name(name)
{
    if (!m_property)
        throw Exception("Could not get property: " + name + " in class " + t.getFullname());

    generateMeta();
}

Access Property::getAccessLevel() const
{
    auto getterAccess = getSetter().getAccessLevel();
    auto setterAccess = getSetter().getAccessLevel();

    if (static_cast<int>(getterAccess) < static_cast<int>(setterAccess))
    {
        return getterAccess;
    }

    return setterAccess;
}

const std::string& Property::getName() const
{
    return m_name;
}

const std::string& Property::getFullname() const
{
    return m_fullname;
}

const std::string& Property::getFullDeclname() const
{
    return m_fullDeclname;
}

const Type& Property::getType() const
{
    return m_type;
}

bool Property::isStatic() const
{
    auto getter = getGetter();
    return getter.isStatic();
}

Method Property::getGetter() const
{
    auto getter = mono_property_get_get_method(m_property);
    return Method(getter);
} 

Method Property::getSetter() const
{
    auto setter = mono_property_get_set_method(m_property);
    return Method(setter);
}

void Property::generateMeta()
{
    auto getter = getGetter();
    m_type = Type(getter.getReturnType());
    m_fullname = m_name;
    
    std::string storage = (isStatic() ? " static " : " ");
    m_fullDeclname = toString(getAccessLevel()) + storage + m_fullname;
}

}
}