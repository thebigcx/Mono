#pragma once

#include <vector>
#include <string>

#include <mono/jit/jit.h>

#include <Mono/Exception.h>
#include <Mono/FieldView.h>

namespace Mono
{

class Object;
class Field;
class Property;
class Method;

class Type
{
public:
    Type() = default;
    Type(MonoClass* c);
    Type(MonoType* t);
    Type(MonoImage* image, const std::string& nameSpace, const std::string& name);

    Method getMethod(const std::string& name);
    MonoClassField* getField(const std::string& name);
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

    std::vector<std::string> getProperties();
    
    bool hasBaseType();
    Type getBaseType();
    bool isDerivedFrom(const Type& type);

    MonoClass* get() const { return m_class; }

    const std::string& getName() const { return m_name; }
    const std::string& getFullname() const { return m_fullname; }
    const std::string& getNamespace() const { return m_namespace; }

    std::size_t getSizeof() const { return m_sizeof; }
    std::size_t getAlignof() const { return m_alignof; }

    int getRank() const { return m_rank; }

    bool isValueType() const { return m_valueType; }

private:
    void generateMeta();

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