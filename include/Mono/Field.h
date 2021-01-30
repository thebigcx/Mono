#pragma once

#include <string>

#include <mono/jit/jit.h>

#include <Mono/Type.h>
#include <Mono/Access.h>

namespace Engine
{
namespace Mono
{

class Field
{
public:
    Field(const Type& type, const std::string& name);

    bool isStatic() const;

    Access getAccessLevel() const;

    const std::string& getName() const { return m_name; }
    const std::string& getFullname() const { return m_fullname; }
    const std::string& getFullDeclname() const { return m_fullDeclname; }

    const Type& getType() const { return m_type; }

    bool isValueType() const;

protected:
    void generateMeta();

    Type m_type;

    MonoClassField* m_field = nullptr;
    MonoVTable* m_typeVtable = nullptr;

    std::string m_name;
    std::string m_fullname;
    std::string m_fullDeclname;
};

}
}