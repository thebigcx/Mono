#include <Mono/Mono.h>
#include <Mono/Compiler.h>
#include <Mono/Assembly.h>
#include <Mono/Object.h>

#include <iostream>

void helloFromCpp()
{
    std::cout << "Hello from C++!\n";
}

int main()
{
    Mono::init("/usr/lib", "/etc", "Application");

    Mono::Compiler compiler("/usr/bin/mcs");
    compiler.buildLibrary("Tests.dll", "Tests.cs");

    Mono::Assembly assembly(Mono::getCurrentDomain(), "Tests.dll");

    Mono::addInternalCall<void()>("Tests.TestClass::helloFromCpp()", MONO_BIND_FN(helloFromCpp));

    Mono::Type type(assembly.getImage(), "Tests", "TestClass");
    Mono::Object object(Mono::getCurrentDomain(), type, "::.ctor(int)", 5);
    auto powMethod = object.getMethod<void(std::string, std::string)>("::add(string,string)");
    powMethod.invokeStatic<void(std::string, std::string)>("Hello, ", "world!");

    object["price"] = 10;
    std::cout << (int)object["price"] << "\n";

    return 0;
}