#pragma once

#include <string>

#include <mono/jit/jit.h>

#include <Mono/Type.h>
#include <Mono/Access.h>
#include <Mono/Domain.h>
#include <Mono/TypeConversion.h>

namespace Mono
{

class Field
{
public:
    Field(const Domain& domain, const Object& object, MonoClassField* field);

    template<typename T>
    void operator=(T&& value)
    {
        auto object = ToMonoConverter<T>::convert(m_domain, std::forward<T>(value));
        mono_field_set_value(m_parent, m_field, (void*)object);
    }

    bool isStatic() const;

    Access getAccessLevel() const;

    const std::string& getName() const { return m_name; }
    const std::string& getFullname() const { return m_fullname; }
    const std::string& getFullDeclname() const { return m_fullDeclname; }

    const Type& getType() const { return m_type; }

    bool isValueType() const;

    template<typename T>
    operator T() const
    {
        return as<T>();
    }

    template<typename T>
    T as() const
    {
        return FromMonoConverter<T>::convert(m_domain, getUnderlyingObject());
    }

    MonoObject* getUnderlyingObject() const
    {
        return mono_field_get_value_object(m_domain, m_field, m_parent);
    }

protected:
    void generateMeta();

    Type m_type;

    MonoDomain* m_domain = nullptr;
    MonoClassField* m_field = nullptr;
    MonoVTable* m_typeVtable = nullptr;
    MonoObject* m_parent = nullptr;

    std::string m_name;
    std::string m_fullname;
    std::string m_fullDeclname;
};

}