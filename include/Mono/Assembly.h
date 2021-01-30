#pragma once

#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Engine
{
namespace Mono
{

class Image;
class Type;

class Assembly
{
public:
    Assembly() = default;
    Assembly(MonoAssembly* assembly)
        : m_assembly(assembly) {}

    Image getImage();
    Type getType(const std::string& nameSpace, const std::string& name);

    MonoAssembly* get() const { return m_assembly; }

private:
    MonoAssembly* m_assembly = nullptr;
};

}
}