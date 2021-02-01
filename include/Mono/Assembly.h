#pragma once

#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include <Mono/Method.h>

namespace Mono
{

class Image;
class Type;
class Domain;

class Assembly
{
public:
    Assembly() = default;
    Assembly(const Domain& domain, const std::string& path);
    Assembly(const Domain& domain, MonoAssembly* assembly)
        : m_assembly(assembly), m_domain(domain.get()) {}

    Method getMethod(const std::string& signature)
    {
        MonoMethodDesc* methodSignature = mono_method_desc_new(signature.c_str(), false);
        MonoMethod* method = mono_method_desc_search_in_image(methodSignature, m_image);

        mono_method_desc_free(methodSignature);

        return Method(Domain(m_domain), method);
    }

    MonoAssembly* get() const { return m_assembly; }

    MonoImage* getImage() const { return m_image; }

private:
    MonoAssembly* m_assembly = nullptr;
    MonoDomain* m_domain = nullptr;
    MonoImage* m_image = nullptr;
};

}