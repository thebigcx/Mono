#pragma once

#include <mono/jit/jit.h>

#include <Mono/Type.h>
#include <Mono/Access.h>

namespace Engine
{
namespace Mono
{

class Property
{
public:
    Property(const Type& type, const std::string& name);

    Access getAccessLevel() const;

    Method getGetter() const;
    Method getSetter() const;

    const std::string& getName() const;
    const std::string& getFullname() const;
    const std::string& getFullDeclname() const;

    const Type& getType() const;

    bool isStatic() const;

private:
    void generateMeta();

    Type m_type;

    MonoProperty* m_property = nullptr;

    std::string m_fullDeclname;
    std::string m_fullname;
    std::string m_name;
};

}
}