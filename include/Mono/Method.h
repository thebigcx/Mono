#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include <Mono/Access.h>
#include <Mono/Domain.h>
#include <Mono/Exception.h>
#include <Mono/TypeConversion.h>

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
    Method(const Domain& domain, MonoMethod* method);
    Method(const Domain& domain, const Type& type, const std::string& name);

    template<typename T, typename... Args>
    decltype(auto) invokeInstance(MonoObject* object, Args&&... args)
    {
        return FunctionInvoker<T>::invoke(*this, object, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args, typename Object>
    decltype(auto) invokeInstance(const Object& object, Args&&... args)
    {
        return invokeInstance<T>(object.get(), std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    decltype(auto) invokeStatic(Args&&... args)
    {
        return invokeInstance<T>(static_cast<MonoObject*>(nullptr), std::forward<Args>(args)...);
    }

    MonoMethod* get() const { return m_method; }

    Type getReturnType() const;
    std::vector<Type> getParamTypes() const;

    const std::string& getName() const;
    const std::string& getFullname() const;
    const std::string& getFullDeclname() const;

    Access getAccessLevel() const;

    bool isStatic() const;
    bool isVirtual() const;

    Domain getDomain() const { return Domain(m_domain); }


private:
    void generateMeta();

protected:
    MonoMethod* m_method = nullptr;
    MonoDomain* m_domain = nullptr;
    MonoMethodSignature* m_methodSignature = nullptr;

    std::string m_name;
    std::string m_fullname;
    std::string m_fullDeclname;
};

}