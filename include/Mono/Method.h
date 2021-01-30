#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include <Mono/Access.h>

namespace Engine
{
namespace Mono
{

class Type;

class Method
{
    friend class Type;
    friend class Property;

public:
    Method(MonoMethod* method);
    Method(const Type& type, const std::string& name);

    Type getReturnType() const;
    std::vector<Type> getParamTypes() const;

    const std::string& getName() const;
    const std::string& getFullname() const;
    const std::string& getFullDeclname() const;

    Access getAccessLevel() const;

    bool isStatic() const;
    bool isVirtual() const;


private:
    void generateMeta();

protected:
    MonoMethod* m_method = nullptr;
    MonoMethodSignature* m_methodSignature = nullptr;

    std::string m_name;
    std::string m_fullname;
    std::string m_fullDeclname;
};

}
}