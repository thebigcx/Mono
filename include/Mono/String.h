#pragma once

#include <string>

#include <Mono/Object.h>

namespace Engine
{
namespace Mono
{

class Domain;

class String : public Object
{
public:
    String(const Object& obj);
    String(const Domain& domain, const std::string& utf8);

    std::string asUtf8() const;
    std::u16string asUtf16() const;
    std::u32string asUtf32() const;

private:
    MonoString* m_string = nullptr;
};

}
}