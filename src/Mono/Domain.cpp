#include <Mono/Domain.h>
#include <Mono/Assembly.h>

#include <mono/metadata/mono-config.h>

#include <iostream>

namespace Engine
{
namespace Mono
{

static MonoDomain* s_domain = nullptr;

bool init(const std::string& domain)
{
    //mono_config_parse("/etc/mono/config");
    mono_set_dirs("/usr/lib", "/etc");

    s_domain = mono_jit_init(domain.c_str());

    return static_cast<bool>(s_domain);
}

void shutdown()
{
    mono_jit_cleanup(s_domain);
}

Domain::Domain(const std::string& name)
{
    m_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);
}

void Domain::create(const std::string& name)
{
    m_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);
}

Assembly Domain::getAssembly(const std::string& file) const
{
    Assembly assembly(mono_domain_assembly_open(m_domain, file.c_str()));
    
    if (!assembly.get())
    {
        std::cout << "[MONO] Could not open assembly: " << file.c_str() << "\n";
    }
    
    return assembly;
}

void Domain::setCurrentDomain(const Domain& dom)
{
    s_currentDomain = &dom;
}

}
}