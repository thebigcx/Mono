#include <Mono/Method.h>
#include <Mono/Type.h>

#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>

#include <cassert>

namespace Mono
{

Method::Method(const Domain& domain, MonoMethod* method)
    : m_domain(domain.get()), m_method(method)
{
    m_method = method;
    generateMeta();
}

Method::Method(const Domain& domain, const Type& t, const std::string& name)
    : m_domain(domain.get())
{
    auto methodDesc = mono_method_desc_new(name.c_str(), 0);
    m_method = mono_method_desc_search_in_class(methodDesc, t.get());
    mono_method_desc_free(methodDesc);
}

Type Method::getReturnType() const
{
    auto type = mono_signature_get_return_type(m_methodSignature);
    return Type(type);
}

std::vector<Type> Method::getParamTypes() const
{
    void* iter = nullptr;
    auto type = mono_signature_get_params(m_methodSignature, &iter);
    std::vector<Type> params;
    while (type)
    {
        params.emplace_back(Type(type));
        type = mono_signature_get_params(m_methodSignature, &iter);
    }

    return params;
}

const std::string& Method::getName() const
{
    return m_name;
}

const std::string& Method::getFullname() const
{
    return m_fullname;
}

const std::string& Method::getFullDeclname() const
{
    return m_fullDeclname;
}

Access Method::getAccessLevel() const
{
    uint32_t acs = mono_method_get_flags(m_method, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

    if (acs == MONO_METHOD_ATTR_PRIVATE)
        return Access::Private;
    else if (acs == MONO_METHOD_ATTR_FAM_AND_ASSEM)
        return Access::ProtectedInternal;
    else if (acs == MONO_METHOD_ATTR_ASSEM)
        return Access::Internal;
    else if (acs == MONO_METHOD_ATTR_FAMILY)
        return Access::Protected;
    else if (acs == MONO_METHOD_ATTR_PUBLIC)
        return Access::Public;

    assert(false);
    return Access::Private;
}

bool Method::isStatic() const
{
    uint32_t flags = mono_method_get_flags(m_method, nullptr);
    return (flags & MONO_METHOD_ATTR_STATIC) != 0;
}

bool Method::isVirtual() const
{
    uint32_t flags = mono_method_get_flags(m_method, nullptr);
    return (flags & MONO_METHOD_ATTR_VIRTUAL) != 0;
}

void Method::generateMeta()
{
    m_methodSignature = mono_method_signature(m_method);
    m_name = mono_method_get_name(m_method);
    m_fullname = mono_method_full_name(m_method, true);
    std::string storage = (isStatic() ? " static " : " ");
    m_fullDeclname = toString(getAccessLevel()) + storage + m_fullname;
}

}