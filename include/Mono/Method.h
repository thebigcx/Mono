#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>

#include <Mono/Access.h>
#include <Mono/Domain.h>
#include <Mono/Exception.h>
#include <Mono/TypeConversionForward.h>
#include <Mono/TypeTraits.h>

namespace Mono
{

class Method;

// Convert all parameters to mono types
template<typename... Args>
void internalInitParams(const Domain& domain, void** current) {} 

template<typename T, typename... Args>
void internalInitParams(const Domain& domain, void** current, T&& type, Args&&... args)
{
    using TypeToConvert = typename std::decay<T>::type;

    auto converted = ToMonoConverter<TypeToConvert>::convert(domain, std::forward<T>(type));

    *current = (void*)converted;
    internalInitParams(domain, current + 1, std::forward<Args>(args)...);
}

template<typename T>
struct FunctionInvoker
{
    template<typename... Args>
    static decltype(auto) invoke(const Method& method, MonoObject* thisObj, Args&&... args)
    {
        return FunctionInvoker<T(Args...)>::invoke(method, thisObj, std::forward<Args>(args)...);
    }
};

// Struct to convert parameters and invoke a C# function
template<typename Ret, typename... Params>
struct FunctionInvoker<Ret(Params...)>
{
    template<typename M, typename... Args>
    static auto invoke(const M& method, MonoObject* thisObj, Args&&... args)
    {
        void** params = new void*[sizeof...(args)];
        internalInitParams(method.getDomain(), params, std::forward<Args>(args)...);

        // Invoke and get result
        MonoObject* result = mono_runtime_invoke(method.get(), thisObj, params, nullptr);
        delete[] params;

        if (!std::is_void<Ret>::value && result == nullptr)
        {
            throw Exception("Method returned null.");
        }

        return FromMonoConverter<Ret>::convert(method.getDomain(), result);
    }

};

class Type;

class Method
{
    friend class Type;
    friend class Property;

public:
    Method(const Domain& domain, MonoMethod* ptr)
        : m_domain(domain.get()), m_method(ptr)
    {
        generateMeta();
    }

    template<typename T, typename... Args>
    decltype(auto) invokeInstance(MonoObject* object, Args&&... args) const
    {
        return FunctionInvoker<T>::invoke(*this, object, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args, typename Object>
    decltype(auto) invokeInstance(const Object& object, Args&&... args) const
    {
        return invokeInstance<T>(object.get(), std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    decltype(auto) invokeStatic(Args&&... args) const
    {
        return invokeInstance<T>(static_cast<MonoObject*>(nullptr), std::forward<Args>(args)...);
    }

    MonoMethod* get() const { return m_method; }

    const std::string& getName() const
    {
        return m_name;
    }

    const std::string& getFullname() const
    {
        return m_fullname;
    }

    const std::string& getFullDeclname() const
    {
        return m_fullDeclname;
    }

    Access getAccessLevel() const
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

    bool isStatic() const
    {
        uint32_t flags = mono_method_get_flags(m_method, nullptr);
        return (flags & MONO_METHOD_ATTR_STATIC) != 0;
    }

    bool isVirtual() const
    {
        uint32_t flags = mono_method_get_flags(m_method, nullptr);
        return (flags & MONO_METHOD_ATTR_VIRTUAL) != 0;
    }

    Domain getDomain() const { return Domain(m_domain); }

    template<typename Signature>
    auto asFunction() const
    {
        using Traits = FunctionTraits<Signature>;
        using ResultType = typename Traits::ResultType;
        using Functor = typename Traits::Type;

        return Functor([method = *this](auto&&... args)
        {
            return method.invokeStatic<ResultType>(std::forward<decltype(args)>(args)...);
        });
    }

private:
    void generateMeta()
    {
        m_methodSignature = mono_method_signature(m_method);
        m_name = mono_method_get_name(m_method);
        m_fullname = mono_method_full_name(m_method, true);
        std::string storage = (isStatic() ? " static " : " ");
        m_fullDeclname = toString(getAccessLevel()) + storage + m_fullname;
    }

protected:
    MonoMethod* m_method = nullptr;
    MonoDomain* m_domain = nullptr;
    MonoMethodSignature* m_methodSignature = nullptr;

    std::string m_name;
    std::string m_fullname;
    std::string m_fullDeclname;
};

}