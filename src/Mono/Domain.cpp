#include <Mono/Domain.h>
#include <Mono/Assembly.h>

#include <mono/metadata/mono-config.h>

#include <iostream>

namespace Mono
{

Domain::Domain(const std::string& name)
{
    m_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);
}

}