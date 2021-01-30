#pragma once

#include <vector>
#include <string>

#include <mono/jit/jit.h>

namespace Engine
{
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

    Object createInstance();

    Method getMethod(const std::string& name);
    Field getField(const std::string& name);
    Property getProperty(const std::string& name) const;
    std::vector<Field> getFields();
    std::vector<Property> getProperties();
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
}