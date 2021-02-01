#include <Mono/Assembly.h>
#include <Mono/Type.h>
#include <Mono/Domain.h>
#include <Mono/Exception.h>

namespace Mono
{

Assembly::Assembly(const Domain& domain, const std::string& path)
    : m_domain(domain.get())
{
    m_assembly = mono_domain_assembly_open(domain.get(), path.c_str());
    m_image = mono_assembly_get_image(m_assembly);
    
    if (!m_assembly)
    {
        std::string what = "[MONO] Could not open assembly: " + path;
        throw Exception(what);
    }
}

}