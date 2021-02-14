#include <Mono/Mono.h>
#include <Mono/Compiler.h>
#include <Mono/Assembly.h>
#include <Mono/Object.h>

#include <iostream>
#include <cstring>

struct Transform
{
    Transform() {}
    Transform(float x_, float y_, float z_)
        : x(x_), y(y_), z(z_) {}

    float x, y, z;
};

namespace Mono
{

template<>
struct ToMonoConverter<Transform>
{
    static auto convert(const Domain& domain, const Transform& transform)
    {
        Assembly* assembly = reinterpret_cast<Assembly*>(getUserPointer());
        Type type(assembly->getImage(), "Engine", "Transform");

        Object object(domain, type);
        object["m_x"] = transform.x;
        object["m_y"] = transform.y;
        object["m_z"] = transform.z;
        return object.get();
    }
};

}

int Log(std::string a)
{
    std::cout << "[CSHARP] " << a << "\n";
    return 1;
}

int LogInt(int a)
{
    std::cout << "[CSHARP] " << a << "\n";
    return 10;
}

Transform transform;

Mono::Object getComponent(std::string type)
{
    static Mono::Object obj;

    if (type == "Transform")
        obj = Mono::Object(Mono::ToMonoConverter<Transform>::convert(Mono::getCurrentDomain(), transform));
        
    return obj;
}

void setTransformX(float x)
{
    transform.x = x;
}

int main()
{
    Mono::init("/usr/lib", "/etc", "Application");

    Mono::Compiler compiler("/usr/bin/mcs");
    compiler.buildLibrary("Tests.dll", "Tests.cs");

    Mono::Assembly assembly(Mono::getCurrentDomain(), "Tests.dll");

    Mono::setUserPointer(&assembly);

    Mono::addInternalCall<int(std::string)>("Tests.TestClass::Log(string)", MONO_BIND_FN(Log));
    Mono::addInternalCall<int(int)>("Tests.TestClass::Log(int)", MONO_BIND_FN(LogInt));
    Mono::addInternalCall<Mono::Object(std::string)>("Engine.GameObject::GetComponent_Internal(string)", MONO_BIND_FN(getComponent));
    Mono::addInternalCall<void(float)>("Engine.Transform::SetTransformX_Internal(single)", MONO_BIND_FN(setTransformX));

    Mono::Type type(assembly.getImage(), "Tests", "TestClass");
    Mono::Object object(Mono::getCurrentDomain(), type);

    auto method = object.getMethod<void()>("::onUpdate()");
    method();

    std::cout << "Transform: " << transform.x << ", " << transform.y << ", " << transform.z << "\n";


    return 0;
}