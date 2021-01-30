#pragma once

#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Engine
{
namespace Mono
{

bool init(const std::string& dom);
void shutdown();

class Assembly;

class Domain
{
public:
    Domain() = default;
    Domain(const std::string& name);
    Domain(MonoDomain* domain)
        : m_domain(domain) {}

    void create(const std::string& name);

    Assembly getAssembly(const std::string& file) const;

    MonoDomain* get() const { return m_domain; }

    static void setCurrentDomain(const Domain& domain);
    static inline const Domain* getCurrentDomain() { return s_currentDomain; }

private:
    MonoDomain* m_domain = nullptr;

    static inline const Domain* s_currentDomain = nullptr;
};

}
}