#pragma once

#include <Mono/Property.h>

namespace Engine
{
namespace Mono
{

template<typename T>
class PropertyInvoker : public Property
{
public:
    // Static and non-static (takes a "this" pointer)
    void setValue(const T& value) const;
    void setValue(const Object& obj, const T& value) const;

    T getValue() const;
    T getValue(const Object& obj) const;

private:
    template<typename Signature>
    friend PropertyInvoker<Signature> makePropertyInvoker(const Property& property);

    PropertyInvoker(const Property& prop)
        : Property(prop)
    {

    }
};

template<typename T>
void PropertyInvoker<T>::setValue(const T& value) const
{
    // Use C#'s get/set methods as a thunk
    auto thunk = makeMethodInvoker<void(const T&)>(getSetter());
    thunk(value);
}

template<typename T>
void PropertyInvoker<T>::setValue(const Object& obj, const T& value) const
{
    auto thunk = makeMethodInvoker<void(const T&)>(getSetter());
    thunk(obj, value);
}

template<typename T>
T PropertyInvoker<T>::getValue() const
{
    auto thunk = makeMethodInvoker<T(void)>(getGetter());
    return thunk();
}

template<typename T>
T PropertyInvoker<T>::getValue(const Object& obj) const
{
    auto thunk = makeMethodInvoker<T(void)>(getGetter());
    return thunk(obj);
}

template<typename T>
PropertyInvoker<T> makePropertyInvoker(const Property& property)
{
    return PropertyInvoker<T>(property);
}

template<typename T>
PropertyInvoker<T> makePropertyInvoker(const Type& type, const std::string& name)
{
    Property property = type.getProperty(name);
    return PropertyInvoker<T>(property);
}

}
}