#pragma once

#include <vector>
#include <string>

#include <mono/jit/jit.h>

#include <Mono/Exception.h>
#include <Mono/FieldView.h>
#include <Mono/Method.h>
#include <Mono/Domain.h>

namespace Mono
{

class Object;
class Field;
class Property;

class Type
{
public:
    Type() = default;

    Type(MonoClass* c)
        : m_class(c)
    {
        generateMeta();
    }

    Type(MonoType* t)
    {
        m_class = mono_class_from_mono_type(t);
        
        generateMeta();
    }

    Type(MonoImage* image, const std::string& nameSpace, const std::string& name)
    {
        m_class = mono_class_from_name(image, nameSpace.c_str(), name.c_str());

        generateMeta();
    }

    MonoMethod* getMonoMethod(const std::string& name)
    {
        auto methodDesc = mono_method_desc_new(name.c_str(), 0);
        MonoMethod* method = mono_method_desc_search_in_class(methodDesc, m_class);
        mono_method_desc_free(methodDesc);

        return method;
    }

    MonoClassField* getField(const std::string& name)
    {
        MonoClassField* field = mono_class_get_field_from_name(m_class, name.c_str());
        if (field == nullptr)
        {
            throw Exception("Could not find field: " + name + " in class: " + m_name);
        }

        return field;
    }

    MonoProperty* getProperty(const std::string& name)
    {
        MonoProperty* prop = mono_class_get_property_from_name(m_class, name.c_str());
        if (prop == nullptr)
        {
            throw Exception("Could not get property: " + name + " in class: " + m_name);
        }

        return prop;
    }

    FieldView getFields() const
    {
        return FieldView(m_class);
    }

    Type getNestingType() const
    {
        return Type(mono_class_get_nesting_type(m_class));
    }

    Type getParentType() const
    {
        return Type(mono_class_get_parent(m_class));
    }
    
    bool hasBaseType()
    {
        return mono_class_get_parent(m_class) != nullptr;
    }

    Type getBaseType()
    {
        auto base = mono_class_get_parent(m_class);
        return Type(base);
    }

    bool isDerivedFrom(const Type& type)
    {
        return mono_class_is_subclass_of(m_class, type.get(), false) != 0;
    }

    MonoClass* get() const { return m_class; }

    const std::string& getName() const { return m_name; }
    const std::string& getFullname() const { return m_fullname; }
    const std::string& getNamespace() const { return m_namespace; }

    std::size_t getSizeof() const { return m_sizeof; }
    std::size_t getAlignof() const { return m_alignof; }

    int getRank() const { return m_rank; }

    bool isValueType() const { return m_valueType; }

private:
    void generateMeta()
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

private:
    MonoClass* m_class = nullptr;
    MonoType* m_type = nullptr;

    std::string m_name;
    std::string m_fullname;
    std::string m_namespace;

    std::size_t m_sizeof = 0;
    std::size_t m_alignof = 0;

    int m_rank = 0;
    bool m_valueType = true;
};

}