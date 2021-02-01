#pragma once

#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Mono
{

class Assembly;

class Domain
{
public:
    Domain(const std::string& name);
    Domain(MonoDomain* domain)
        : m_domain(domain) {}

    MonoDomain* get() const { return m_domain; }

private:
    MonoDomain* m_domain = nullptr;
};

}