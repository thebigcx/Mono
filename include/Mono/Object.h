#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

#include <Mono/Type.h>
#include <Mono/Domain.h>
#include <Mono/Method.h>
#include <Mono/Exception.h>

namespace Mono
{

class Object
{
public:
    class Field
    {
    public:
        Field(MonoDomain* domain, MonoObject* object, MonoClassField* field)
            : m_field(field), m_parent(object), m_domain(domain)
        {
            
        }

        template<typename T>
        void operator=(T&& value)
        {
            auto object = ToMonoConverter<T>::convert(m_domain, std::forward<T>(value));
            mono_field_set_value(m_parent, m_field, &object);
        }

        template<typename T>
        operator T() const
        {
            return as<T>();
        }

        template<typename T>
        T as() const
        {
            return FromMonoConverter<T>::convert(m_domain, getUnderlyingObject());
        }

        MonoObject* getUnderlyingObject() const
        {
            return mono_field_get_value_object(m_domain, m_field, m_parent);
        }

    private:
        MonoDomain* m_domain = nullptr;
        MonoClassField* m_field = nullptr;
        MonoObject* m_parent = nullptr;
    };


public:
    Object(const Domain& domain, const Type& type)
        : m_domain(domain.get())
    {
        m_object = mono_object_new(domain.get(), type.get());
        mono_runtime_object_init(m_object);
    }

    Object(MonoObject* obj)
        : m_object(obj) {}

    template<typename... Args>
    Object(const Domain& domain, const Type& type, const std::string& ctor, Args&&... args)
        : m_domain(domain.get())
    {
        m_object = mono_object_new(domain.get(), type.get());

        MonoMethodDesc* ctorDesc = mono_method_desc_new(ctor.c_str(), false);
        if (!ctorDesc)
        {
            throw Exception("Invalid constructor signature: " + ctor);
        }

        MonoMethod* ctorMethod = mono_method_desc_search_in_class(ctorDesc, type.get());
        mono_method_desc_free(ctorDesc);

        Method constructor(domain, ctorMethod);
        constructor.invokeInstance<void(Args...)>(*this, std::forward<Args>(args)...);
    }

    MonoMethod* getMonoMethod(const std::string& name)
    {
        return getType().getMonoMethod(name);
    }

    template<typename Signature>
    auto getMethod(const std::string& name)
    {
        using Traits = InternalGetFunctionTraits<Signature>;

        auto methodType = getMonoMethod(name);
        auto functor = [f = Method(m_domain, methodType), o = m_object](auto&&... args) mutable -> typename Traits::ResultType
        {
            return f.invokeInstance<Signature>(o, std::forward<decltype(args)>(args)...);
        };

        return typename Traits::Type(std::move(functor));
    }

    template<typename Signature>
    auto getStaticMethod(const std::string& name)
    {
        using Traits = InternalGetFunctionTraits<Signature>;

        auto methodType = getMonoMethod(name);
        auto functor = [f = Method(m_domain, methodType)](auto&&... args) mutable -> typename Traits::ResultType
        {
            return f.invokeStatic<Signature>(std::forward<decltype(args)>(args)...);
        };

        return typename Traits::Type(std::move(functor));
    }

    Field operator[](const std::string& fieldName) const
    {
        return Field(m_domain, m_object, getType().getField(fieldName));
    }

    template<typename T>
    void setProperty(const std::string& name, const T& value) const
    {
        MonoProperty* prop = getType().getProperty(name);
        MonoObject* ex = nullptr;
        void** params = new void*[1];
        params[0] = (void*)ToMonoConverter<T>::convert(Domain(m_domain), value);

        mono_property_set_value(prop, m_object, params, &ex);
        if (ex)
        {
            throw Exception("Error while setting property value: " + name);
        }

        delete params;
    }

    Object getProperty(const std::string& name) const
    {
        MonoProperty* prop = getType().getProperty(name);
        MonoObject* ex = nullptr;

        MonoObject* result = mono_property_get_value(prop, m_object, nullptr, &ex);
        if (ex)
        {
            throw Exception("Error while getting property value: " + name);
        }

        return Object(result);
    }

    template<typename T>
    T getProperty(const std::string& name) const
    {
        return getProperty(name).as<T>();
    }

    Type getType() const
    {
        MonoClass* cl = mono_object_get_class(m_object);
        return Type(cl);
    }

    MonoObject* get() const { return m_object; }

    template<typename T>
    T as() const
    {
        assert(m_object != nullptr);
        return FromMonoConverter<T>::convert(Domain(m_domain), m_object);
    }

    std::string toString() const
    {
        return as<std::string>();
    }

protected:
    MonoObject* m_object = nullptr;
    MonoDomain* m_domain = nullptr;
};

}