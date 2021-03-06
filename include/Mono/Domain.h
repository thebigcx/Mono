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
    Domain(const std::string& name)
    {
        m_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);
    }

    Domain(MonoDomain* domain)
        : m_domain(domain) {}

    MonoDomain* get() const { return m_domain; }

private:
    MonoDomain* m_domain = nullptr;
};

}